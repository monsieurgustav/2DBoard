//
//  Events.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 06/10/13.
//
//

#include "Events.h"
#include "IWidget.h"
#include "Helpers.h"
#include "Loader.h"
#include "Level.h"

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/audio/Output.h"


void ev::setTrigger(Board & board, ci::Vec2i position, int triggerId)
{
    board.cell(position.x, position.y).setTriggerId(triggerId);
}


void ev::removeTrigger(Board & board, ci::Vec2i position)
{
    board.cell(position.x, position.y).removeTrigger();
}


void ev::setGround(Board & board, ci::Vec2i position, int tileId, bool blocking)
{
    board.cell(position.x, position.y).setGroundId(tileId, blocking);
}


void ev::setLayer(Board & board, ci::Vec2i position, int tileId)
{
    board.cell(position.x, position.y).setLayerId(tileId);
}


void ev::removeLayer(Board & board, ci::Vec2i position)
{
    board.cell(position.x, position.y).removeLayer();
}


void ev::setPlayerTiles(Actor & player, int stillTile, ci::Vec4i movingTiles)
{
    player.setTileId(stillTile, DIR_NONE);
    player.setTileId(movingTiles.x, DIR_UP);
    player.setTileId(movingTiles.y, DIR_RIGHT);
    player.setTileId(movingTiles.z, DIR_DOWN);
    player.setTileId(movingTiles.w, DIR_LEFT);
}


Level ev::loadLevel(ci::app::App * app, const std::string & filename)
{
    return loadFrom(app, app->loadAsset(filename));
}


namespace
{
    class TextureWidget : public IWidget
    {
        ci::gl::TextureRef texture;
    public:
        TextureWidget(ci::gl::TextureRef tex) : texture(tex)
        { }

        virtual void afterDraw(ci::Vec2i windowSize) override
        {
            const int ratio = reduce_min(windowSize/texture->getSize());
            auto dst = texture->getBounds();
            dst.expand((ratio-1)*texture->getWidth()/2,
                       (ratio-1)*texture->getHeight()/2);
            dst.moveULTo((windowSize-dst.getSize())/2);
            ci::gl::draw(texture, dst);
        }
    };

    class TimerTextureWidget : public TextureWidget
    {
        ci::Anim<int> timer;
    public:
        TimerTextureWidget(ci::Timeline & timeline, ci::gl::TextureRef tex,
                           float duration)
        : TextureWidget(tex), timer(0)
        {
            timeline.apply(&timer, 1, duration);
        }

        virtual State update() override
        {
            return timer == 0 ? KEEP : REMOVE;
        }
        
        virtual bool keyDown(ci::app::KeyEvent event) override
        {
            // modal
            return true;
        }
    };

    class ModalTextureWidget : public TextureWidget
    {
        State state;
    public:
        ModalTextureWidget(ci::gl::TextureRef tex)
        : TextureWidget(tex), state(KEEP)
        { }

        virtual State update() override
        {
            return state;
        }

        virtual bool keyDown(ci::app::KeyEvent event) override
        {
            state = (event.getCode() == ci::app::KeyEvent::KEY_RETURN)
                    ? REMOVE : KEEP;

            // modal
            return true;
        }
    };
}
void ev::displayImage(ci::app::App * app, Level & level,
                      const std::string & filename, float duration)
{
    ci::gl::TextureRef tex = loadTexture(app, filename);
    auto w = std::make_shared<TimerTextureWidget>(app->timeline(), tex, duration);
    level.pendingWidgets.push_back(w);
}


void ev::displayPrompt(ci::app::App * app, Level & level,
                      const std::string & filename)
{
    ci::gl::TextureRef tex = loadTexture(app, filename);
    auto w = std::make_shared<ModalTextureWidget>(tex);
    level.pendingWidgets.push_back(w);
}


namespace
{
    class SolidColorWidget : public IWidget
    {
        static ci::gl::GlslProgRef sProgram;
        ci::Color mColor;
        ci::Anim<float> mAlpha;

    public:
        SolidColorWidget(ci::Color color) : mColor(color)
        { }

        ci::Anim<float> & alpha()
        {
            return mAlpha;
        }

        virtual State update() override
        {
            return mAlpha.isComplete() ? REMOVE : KEEP;
        }

        virtual void afterDraw(ci::Vec2i windowSize) override
        {
            if(!sProgram)
            {
                init();
            }
            sProgram->bind();
            sProgram->uniform("color", ci::ColorA(mColor, mAlpha));
            ci::gl::drawSolidRect(ci::Rectf(-1.f, -1.f, 1.f, 1.f));
            sProgram->unbind();
        }

        virtual bool keyDown(ci::app::KeyEvent event) override
        {
            // modal
            return true;
        }
    private:
        static void init()
        {
            sProgram = ci::gl::GlslProg::create(
                "void main() { gl_Position = gl_Vertex; }"
                ,
                "uniform vec4 color;\n"
                "void main() { gl_FragColor = color; }");
        }
    };
    ci::gl::GlslProgRef SolidColorWidget::sProgram;
}

void ev::fadeInColor(ci::app::App * app, Level & level, ci::Color color, float duration)
{
    auto w = std::make_shared<SolidColorWidget>(color);
    app->timeline().apply(&w->alpha(), 1.f, 0.f, duration, &ci::easeInOutQuad);
    level.pendingWidgets.push_back(w);
}


void ev::fadeOutColor(ci::app::App * app, Level & level, ci::Color color, float duration)
{
    auto w = std::make_shared<SolidColorWidget>(color);
    app->timeline().apply(&w->alpha(), 0.f, 1.f, duration, &ci::easeInOutQuad);
    level.pendingWidgets.push_back(w);
}


void ev::playSound(ci::app::App * app, Level & level, const std::string &filename, bool loop)
{
    auto source = ci::audio::load(app->loadAsset(filename));
    auto track = ci::audio::Output::addTrack(source);
    track->setLooping(loop);
    
    auto sound = std::make_pair(track, ci::Anim<float>(1.f));
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        found->second.first->stop();
        found->second = sound;
    }
    else
    {
        level.sounds[filename] = sound;
    }
}


void ev::stopSound(Level & level, const std::string & filename)
{
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        found->second.first->stop();
        level.sounds.erase(found);
    }
}


void ev::fadeInSound(ci::app::App * app, Level & level, const std::string & filename, float duration)
{
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        app->timeline().apply(&found->second.second, 0.f, 1.f, duration, &ci::easeInOutQuad);
    }
}


void ev::fadeOutSound(ci::app::App * app, Level & level, const std::string & filename, float duration)
{
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        app->timeline().apply(&found->second.second, 0.f, duration, &ci::easeInOutQuad);
    }
}


namespace
{
    class WaitWidget : public IWidget
    {
        ci::Anim<int> timer;
        bool modal_;
    public:
        WaitWidget(ci::Timeline & timeline, float duration, bool modal)
         : timer(0), modal_(modal)
        {
            timeline.apply(&timer, 1, duration);
        }

        virtual State update() override
        {
            return timer == 0 ? KEEP : REMOVE;
        }

        virtual bool keyDown(ci::app::KeyEvent event) override
        {
            return modal_;
        }
    };
}
void ev::wait(ci::app::App * app, Level & level, float duration, bool modal)
{
    level.pendingWidgets.push_back(std::make_shared<WaitWidget>(app->timeline(),
                                                                duration,
                                                                modal));
}


namespace
{
#define SERIAL_FORWARD_CALL(FUNCNAME, PARAMTYPE, PARAMNAME)                    \
    virtual void FUNCNAME(PARAMTYPE PARAMNAME) override                        \
    {                                                                          \
        std::for_each(children.begin(), children.end(),                        \
                      [PARAMNAME] (IWidgetPtr &w) { w->FUNCNAME(PARAMNAME); });\
    }

#define SERIAL_FORWARD_CALL_FILTERED(FUNCNAME, PARAMTYPE, PARAMNAME)           \
    virtual bool FUNCNAME(PARAMTYPE PARAMNAME) override                        \
    {                                                                          \
        if(std::any_of(children.begin(), children.end(),                       \
                       [PARAMNAME] (IWidgetPtr &w) { return w->FUNCNAME(PARAMNAME); })) \
        {                                                                      \
            return true;                                                       \
        }                                                                      \
        return false;                                                          \
    }

    class SerialWidget : public IWidget
    {
        ci::app::App * app;
        Level & level;

        std::queue<EventManager::Event> events;
        std::deque<IWidgetPtr> children;

    public:

        SerialWidget(ci::app::App * a, Level & l,
                     std::queue<EventManager::Event> evts,
                     std::deque<IWidgetPtr> widgets)
        : app(a), level(l), events(std::move(evts)),
          children(std::move(widgets))
        {}

        virtual State update() override
        {
            // forward update call
            auto newEnd = std::remove_if(children.begin(), children.end(),
                                         [] (IWidgetPtr &w)
                                         {
                                             return w->update() == IWidget::REMOVE;
                                         });
            children.erase(newEnd, children.end());

            if(!children.empty())
            {
                return KEEP;
            }

            // exec next events
            exec(app, level, events, children);
            return children.empty() ? REMOVE : KEEP;
        }

        SERIAL_FORWARD_CALL(beforeDraw, cinder::Vec2i, windowSize)
        SERIAL_FORWARD_CALL(afterDraw, cinder::Vec2i, windowSize)
        SERIAL_FORWARD_CALL_FILTERED(keyDown, ci::app::KeyEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(keyUp, ci::app::KeyEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(mouseDown, ci::app::MouseEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(mouseUp, ci::app::MouseEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(mouseWheel, ci::app::MouseEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(mouseMove, ci::app::MouseEvent, event)
        SERIAL_FORWARD_CALL_FILTERED(mouseDrag, ci::app::MouseEvent, event)

        static void exec(ci::app::App * app, Level & level,
                         std::queue<EventManager::Event> & events,
                         std::deque<IWidgetPtr> & newWidgets)
        {
            while(!events.empty())
            {
                const auto event = events.front();
                events.pop();
                const auto previousLevel = level.current;
                const auto previousSize = level.pendingWidgets.size();
                event(app, level);  // event call
                const auto currentLevel = level.current;
                const auto currentSize = level.pendingWidgets.size();
                if(currentLevel != previousLevel)
                {
                    return;
                }
                if(currentSize > previousSize)
                {
                    auto beginNew = level.pendingWidgets.begin()+previousSize;
                    auto endNew = level.pendingWidgets.end();
                    newWidgets.insert(newWidgets.end(), beginNew, endNew);
                    level.pendingWidgets.erase(beginNew, endNew);
                    return;
                }
            }
        }
    };

#undef SERIAL_FORWARD_CALL
#undef SERIAL_FORWARD_CALL_FILTERED
}

void ev::serial(ci::app::App * app, Level & level,
                std::queue<EventManager::Event> events)
{
    std::deque<IWidgetPtr> newWidgets;
    SerialWidget::exec(app, level, events, newWidgets);
    if(!newWidgets.empty())
    {
        // The serial widget takes ownership of pending widgets.
        // It is responsible for calling updates, etc.

        // Rational: serial widget update must be called after
        // its children widget updates, or it may be 1 frame
        // late.
        // Include the updates of children widget in the parent
        // widget is the adopted solution.
        auto w = std::make_shared<SerialWidget>(app, level,
                                                std::move(events),
                                                std::move(newWidgets));
        level.pendingWidgets.push_back(w);
    }
}
