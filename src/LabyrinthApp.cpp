#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"

#include "TerrainLoader.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class LabyrinthApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

Anim<Vec2f> center;
Terrain gTerrain;

void LabyrinthApp::setup()
{
    center = Vec2f(5, 5);
    timeline().apply(&center, Vec2f(250, 250), 5.f);
    gTerrain = loadFrom(loadResource("terrain.ter"));
}

void LabyrinthApp::mouseDown( MouseEvent event )
{
}

void LabyrinthApp::update()
{
}

void LabyrinthApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::drawSolidCircle(center, 2.f);
}

CINDER_APP_NATIVE( LabyrinthApp, RendererGl )
