#include "TerrainLoader.h"
#include "Actor.h"
#include "EventManager.h"
#include "Drawer.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"


using namespace ci;
using namespace ci::app;
using namespace std;


EventManager gEventManager;
Terrain gTerrain;
Actor * gActor = nullptr;
Drawer * gDrawer = nullptr;


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
    gl::enableAlphaTest();
    gl::disableDepthRead();
    
    gDrawer = new Drawer(32);
    gDrawer->setWindowSize(getWindowWidth(), getWindowHeight());

    // ground square tiles
    auto groundTex = gl::Texture::create(loadImage(loadAsset("ground.png")));
    groundTex->setMagFilter(GL_NEAREST);
    gDrawer->setTile(1, groundTex, 32, 12);
    gDrawer->setTile(2, groundTex, 32, 10);

    // actor tiles: ratio=48/32
    auto charTexture = gl::Texture::create(loadImage(loadAsset("actor.png")));
    charTexture->setMagFilter(GL_NEAREST);
    gDrawer->setTile(3, charTexture, 48, 12, 16);
    gDrawer->setTile(4, charTexture, 48, 0, 4);
    gDrawer->setTile(5, charTexture, 48, 8, 12);
    gDrawer->setTile(6, charTexture, 48, 4, 8);
    gDrawer->setTile(7, charTexture, 48, 1);

    gTerrain = loadFrom(loadResource("terrain.ter"));

    gActor = new Actor(timeline());
    gActor->setTileId(3, DIR_UP);
    gActor->setTileId(4, DIR_DOWN);
    gActor->setTileId(5, DIR_LEFT);
    gActor->setTileId(6, DIR_RIGHT);
    gActor->setTileId(7, DIR_NONE);
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
    gDrawer->draw(gTerrain, *gActor);
}

CINDER_APP_NATIVE( LabyrinthApp, RendererGl )
