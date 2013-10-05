#include "TerrainLoader.h"
#include "Actor.h"
#include "EventManager.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"


using namespace ci;
using namespace ci::app;
using namespace std;


EventManager gEventManager;
Terrain gTerrain;
Actor * gActor;


class LabyrinthApp : public AppNative {
  public:
	void setup();
	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);
	void update();
	void draw();
};


void LabyrinthApp::setup()
{
    gTerrain = loadFrom(loadResource("terrain.ter"));

    gEventManager.setEvent(1, [this] { console() << "event" << std::endl; });

    gActor = new Actor(timeline());
    gActor->setStartPosition(ci::Vec2i(3, 3));
    gActor->setFinishMoveCallback([] (ci::Vec2i position)
        {
            int trigger = gTerrain.cell(position.x, position.y).triggerId();
            gEventManager.runEvent(trigger);
        });
}

void LabyrinthApp::keyDown(KeyEvent event)
{
    Direction direction = DIR_NONE;
    switch(event.getCode())
    {
        case KeyEvent::KEY_UP:
            direction = DIR_UP;
            break;
        case KeyEvent::KEY_DOWN:
            direction = DIR_DOWN;
            break;
        case KeyEvent::KEY_LEFT:
            direction = DIR_LEFT;
            break;
        case KeyEvent::KEY_RIGHT:
            direction = DIR_RIGHT;
            break;
    }

    const ci::Vec2i actorPos = gActor->logicalPosition();
    direction = (direction & gTerrain.availableMoves(actorPos.x, actorPos.y)) ?
                direction : DIR_NONE;
    gActor->setNextMove(direction);
}

void LabyrinthApp::keyUp(KeyEvent event)
{
    gActor->setNextMove(DIR_NONE);
}

void LabyrinthApp::update()
{
}

void LabyrinthApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::drawSolidCircle(Vec2f(200, 200)+ci::Vec2f(50,50)*gActor->animatedPosition(), 2.f);
}

CINDER_APP_NATIVE( LabyrinthApp, RendererGl )
