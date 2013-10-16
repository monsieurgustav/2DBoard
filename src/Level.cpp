//
//  Level.cpp
//  Labyrinth
//
//  Created by Guillaume on 12/10/13.
//
//

#include "Level.h"

#include "cinder/app/App.h"


Level::Level(Board && b, Actor && p, Drawer && d, EventManager && m)
: board(b), player(p), drawer(d), eventManager(m)
{ }

Level::Level(Level && other)
: board(std::move(other.board)),
  player(std::move(other.player)),
  drawer(std::move(other.drawer)),
  eventManager(std::move(other.eventManager)),
  widgets(std::move(other.widgets))
{ }

Level & Level::operator=(Level && other)
{
    board = std::move(other.board);
    player = std::move(other.player);
    drawer = std::move(other.drawer);
    eventManager = std::move(other.eventManager);
    widgets = std::move(other.widgets);
    return *this;
}

void Level::prepare(ci::app::App * app)
{
    player.setFinishMoveCallback([app, this] (ci::Vec2i position)
        {
            int trigger = board .cell(position.x, position.y).triggerId();
            eventManager.runEvent(trigger, app, *this);
        });

    drawer.setWindowSize(app->getWindowSize());
    
    // run the initial event.
    eventManager.runEvent(0, app, *this);
}

void Level::destroy(ci::app::App * app)
{

}
