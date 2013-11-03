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
#include "cinder/audio/Output.h"


void ev::setTrigger(Board & board, ci::Vec2i position, int triggerId)
{
    board.cell(position.x, position.y).setTriggerId(triggerId);
}


void ev::setGround(Board & board, ci::Vec2i position, int tileId, bool blocking)
{
    board.cell(position.x, position.y).setGroundId(tileId, blocking);
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
    IWidget * w = new TimerTextureWidget(app->timeline(), tex, duration);
    level.pendingWidgets.push_back(IWidgetPtr(w));
}


void ev::playSound(ci::app::App * app, Level & level, const std::string &filename, bool loop)
{
    auto source = ci::audio::load(app->loadAsset(filename));
    auto track = ci::audio::Output::addTrack(source);
    track->setLooping(loop);
    
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        found->second->stop();
        found->second = track;
    }
    else
    {
        level.sounds[filename] = track;
    }
}


void ev::stopSound(Level & level, const std::string & filename)
{
    auto found = level.sounds.find(filename);
    if(found != level.sounds.end())
    {
        found->second->stop();
        level.sounds.erase(found);
    }
}


namespace
{
    class WaitWidget : public IWidget
    {
        ci::Anim<int> timer;
    public:
        WaitWidget(ci::Timeline & timeline, float duration) : timer(0)
        {
            timeline.apply(&timer, 1, duration);
        }

        virtual State update() override
        {
            return timer == 0 ? KEEP : REMOVE;
        }
    };
}
void ev::wait(ci::app::App * app, Level & level, float duration)
{
    level.pendingWidgets.push_back(std::make_shared<WaitWidget>(app->timeline(),
                                                                duration));
}


namespace
{
    class SerialWidget : public IWidget
    {
        ci::app::App * app;
        Level & level;

        std::queue<EventManager::Event> events;
        std::deque<std::weak_ptr<IWidget>> activeWidgets;

    public:

        SerialWidget(ci::app::App * a, Level & l,
                     const std::queue<EventManager::Event> &evts,
                     size_t firstWidget)
        : app(a), level(l), events(evts)
        {
            trackWidgetFrom(firstWidget);
        }

        virtual State update() override
        {
            if(std::all_of(activeWidgets.begin(), activeWidgets.end(),
                           [](const std::weak_ptr<IWidget> &w)
                           { return w.expired(); }))
            {
                activeWidgets.clear();

                // exec next events if the previous one expired.
                while(!events.empty())
                {
                    const auto event = events.front();
                    events.pop();
                    const auto previousSize = level.pendingWidgets.size();
                    event(app, level);  // event call
                    const auto currentSize = level.pendingWidgets.size();
                    if(currentSize > previousSize)
                    {
                        trackWidgetFrom(previousSize);
                        return KEEP;
                    }
                }
            }
            return events.empty() ? REMOVE : KEEP;
        }

        virtual void afterDraw(cinder::Vec2i) override
        {

        }

        void trackWidgetFrom(size_t index)
        {
            assert(index < level.pendingWidgets.size());
            activeWidgets.insert(activeWidgets.end(),
                                 level.pendingWidgets.begin()+index,
                                 level.pendingWidgets.end());
        }
    };
}
void ev::serial(ci::app::App * app, Level & level,
                std::queue<EventManager::Event> events)
{
    while(!events.empty())
    {
        auto event = events.front();
        events.pop();
        const auto previousSize = level.pendingWidgets.size();
        event(app, level);  // event call
        const auto currentSize = level.pendingWidgets.size();
        if(currentSize > previousSize)
        {
            auto w = std::make_shared<SerialWidget>(app, level,
                                                    std::move(events),
                                                    previousSize);
            level.pendingWidgets.push_back(w);
            return;
        }
    }
}
