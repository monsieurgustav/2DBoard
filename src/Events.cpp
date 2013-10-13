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
