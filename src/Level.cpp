//
//  Level.cpp
//  Labyrinth
//
//  Created by Guillaume on 12/10/13.
//
//

#include "Level.h"

#include "cinder/app/App.h"


void Level::prepare(ci::app::App * app)
{
    player.setFinishMoveCallback([this] (ci::Vec2i position)
        {
            int trigger = terrain.cell(position.x, position.y).triggerId();
            eventManager.runEvent(trigger);
        });

    drawer.setWindowSize(app->getWindowWidth(), app->getWindowHeight());
}

void Level::destroy(ci::app::App * app)
{

}
