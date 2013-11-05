#include "Loader.h"
#include "EventManager.h"
#include "Level.h"
#include "Events.h"
#include "IWidget.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/audio/Output.h"
#include "cinder/ImageIo.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"

#include "FileWatcher/FileWatcher.h"

using namespace ci;
using namespace ci::app;
using namespace std;


Direction gActorDirection = DIR_NONE;
LevelPtr gLevel;
FW::FileWatcher gWatcher;

class LabyrinthApp : public AppNative
{
public:
	void setup();
    void shutdown();
	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);
    virtual void resize() override;
	void update();
	void draw();
};

namespace {
    class LevelWatch : public FW::FileWatchListener
    {
        ci::app::App *mApp;
    public:
        LevelWatch(ci::app::App * app) : mApp(app)
        {}

        virtual void handleFileAction(FW::WatchID watchid,
                                      const FW::String& dir,
                                      const FW::String& filename,
                                      FW::Action action) override
        {
            if(action != FW::Actions::Modified)
            {
                return;
            }
            // filename is:
            //  - filename.ext on Windows
            //  - absolutepath/filename.ext on MacOSX
            const auto file = boost::filesystem::path(filename).filename();
            if(gLevel->source->getFilePath().filename() == file)
            {
                const auto playerPos = gLevel->player.logicalPosition();

                gLevel->destroy(mApp);
                try {
                    *gLevel = std::move(loadFrom(mApp, loadAsset(file)));
                    gLevel->player.setStartPosition(playerPos);
                }
                catch(BadFormatException &) {
                    // do nothing
                }
                gLevel->prepare(mApp);
            }
        }
    };
}

void LabyrinthApp::setup()
{
    gl::enableAlphaBlending();
    gl::enableAlphaTest(0.01f);
    gl::disableDepthRead();
    gl::disable(GL_MULTISAMPLE);

    const auto startLevel = loadAsset("start.lvl");

    gLevel = LevelPtr(new Level(loadFrom(this, startLevel)));
    gLevel->source = startLevel;
    gLevel->prepare(this);

    const auto assetPath = startLevel->getFilePath().parent_path();
    gWatcher.addWatch(assetPath.string(), new LevelWatch(this));
}

void LabyrinthApp::shutdown()
{
    gLevel.reset();
}

void LabyrinthApp::keyDown(KeyEvent event)
{
    if(std::any_of(gLevel->widgets.begin(), gLevel->widgets.end(),
                   [&event] (IWidgetPtr &w) { return w->keyDown(event); }))
    {
        return;
    }

    gActorDirection = DIR_NONE;
    switch(event.getCode())
    {
        case KeyEvent::KEY_UP:
            gActorDirection = DIR_UP;
            break;
        case KeyEvent::KEY_DOWN:
            gActorDirection = DIR_DOWN;
            break;
        case KeyEvent::KEY_LEFT:
            gActorDirection = DIR_LEFT;
            break;
        case KeyEvent::KEY_RIGHT:
            gActorDirection = DIR_RIGHT;
            break;
    }
}

void LabyrinthApp::keyUp(KeyEvent event)
{
    if(std::any_of(gLevel->widgets.begin(), gLevel->widgets.end(),
                   [&event] (IWidgetPtr &w) { return w->keyDown(event); }))
    {
        return;
    }

    gActorDirection = DIR_NONE;
}

void LabyrinthApp::resize()
{
    gLevel->drawer.setWindowSize(getWindowSize());
}

void LabyrinthApp::update()
{
    // check file update
    gWatcher.update();

    // sound volume
    std::for_each(gLevel->sounds.begin(), gLevel->sounds.end(),
                  [] (decltype(*gLevel->sounds.begin()) &sound)
                  {
                      sound.second.first->setVolume(sound.second.second);
                  });

    // include pending widgets
    gLevel->widgets.insert(gLevel->widgets.end(),
                           gLevel->pendingWidgets.begin(),
                           gLevel->pendingWidgets.end());
    gLevel->pendingWidgets.clear();

    // update
    auto newEnd = std::remove_if(gLevel->widgets.begin(), gLevel->widgets.end(),
                                 [] (IWidgetPtr &w)
                                 {
                                     return w->update() == IWidget::REMOVE;
                                 });
    gLevel->widgets.erase(newEnd, gLevel->widgets.end());

    if(!gLevel->drawer.scrolling())
    {
        const ci::Vec2i actorPos = gLevel->player.logicalPosition();
        unsigned char available = gLevel->board.availableMoves(actorPos.x,
                                                               actorPos.y);
        Direction direction = (gActorDirection & available) ? gActorDirection : DIR_NONE;
        gLevel->player.setNextMove(direction);
    }
    else
    {
        gLevel->player.setNextMove(DIR_NONE);
    }
}

void LabyrinthApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

    std::for_each(gLevel->widgets.begin(), gLevel->widgets.end(),
                  [this] (IWidgetPtr &w) { w->beforeDraw(getWindowSize()); });

    gLevel->drawer.draw(gLevel->board, gLevel->player);

    std::for_each(gLevel->widgets.begin(), gLevel->widgets.end(),
                  [this] (IWidgetPtr &w) { w->afterDraw(getWindowSize()); });
}

CINDER_APP_NATIVE( LabyrinthApp, RendererGl(RendererGl::AA_NONE) )
