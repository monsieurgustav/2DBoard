#include "Loader.h"
#include "EventManager.h"
#include "Level.h"
#include "Events.h"
#include "IWidget.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"


typedef std::unique_ptr<IWidget> IWidgetPtr;


using namespace ci;
using namespace ci::app;
using namespace std;


Direction gActorDirection = DIR_NONE;
Level * gLevel;

class LabyrinthApp : public AppNative
{
public:
	void setup();
	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);
	void update();
	void draw();

private:
    std::deque<IWidgetPtr> mWidgets;
};


void LabyrinthApp::setup()
{
    gl::enableAlphaTest();
    gl::disableDepthRead();
    gl::disable(GL_MULTISAMPLE);

    gLevel = new Level(loadFrom(this, loadAsset("terrain.ter")));
    gLevel->prepare(this);
}

void LabyrinthApp::keyDown(KeyEvent event)
{
    if(std::any_of(mWidgets.begin(), mWidgets.end(),
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
    if(std::any_of(mWidgets.begin(), mWidgets.end(),
                   [&event] (IWidgetPtr &w) { return w->keyDown(event); }))
    {
        return;
    }

    gActorDirection = DIR_NONE;
}

void LabyrinthApp::update()
{
    auto newEnd = std::remove_if(mWidgets.begin(), mWidgets.end(),
                                 [] (IWidgetPtr &w)
                                 {
                                     return w->update() == IWidget::REMOVE;
                                 });
    mWidgets.erase(newEnd, mWidgets.end());

    const ci::Vec2i actorPos = gLevel->player.logicalPosition();
    unsigned char available = gLevel->terrain.availableMoves(actorPos.x, actorPos.y);
    Direction direction = (gActorDirection & available) ? gActorDirection : DIR_NONE;
    gLevel->player.setNextMove(direction);
}

void LabyrinthApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

    std::for_each(mWidgets.begin(), mWidgets.end(),
                  [this] (IWidgetPtr &w) { w->beforeDraw(getWindowSize()); });

    gLevel->drawer.draw(gLevel->terrain, gLevel->player);

    std::for_each(mWidgets.begin(), mWidgets.end(),
                  [this] (IWidgetPtr &w) { w->afterDraw(getWindowSize()); });
}

CINDER_APP_NATIVE( LabyrinthApp, RendererGl(RendererGl::AA_NONE) )
