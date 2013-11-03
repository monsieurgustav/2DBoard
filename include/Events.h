//
//  Events.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 06/10/13.
//
//

#ifndef __Labyrinth__Events__
#define __Labyrinth__Events__

#include "EventManager.h"

#include <memory>
#include <string>
#include <queue>

class IWidget;
class Board;
struct Level;
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

    void displayImage(ci::app::App * app, Level & level,
                      const std::string & filename, float duration);

    void playSound(ci::app::App * app, Level & level, const std::string & filename, bool loop);

    void stopSound(Level & level, const std::string & filename);

    // utilities
    void wait(ci::app::App * app, Level & level, float duration);

    void serial(ci::app::App * app, Level & level,
                std::queue<EventManager::Event> events);
}

#endif
