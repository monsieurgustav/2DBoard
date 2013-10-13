//
//  Events.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 06/10/13.
//
//

#ifndef __Labyrinth__Events__
#define __Labyrinth__Events__

#include <memory>
#include <string>

class IWidget;
class Board;
class Level;
class Actor;

namespace cinder {
    namespace app {
        class App;
    }
}
namespace ci = cinder;


namespace ev
{
    void setTrigger(Board & board, ci::Vec2i position, int triggerId);

    void setGround(Board & board, ci::Vec2i position, int tileId, bool blocking);

    void setPlayerTiles(Actor & player, int stillTile, ci::Vec4i movingTiles);

    Level loadLevel(ci::app::App * app, const std::string & filename);
}

#endif
