//
//  Level.h
//  Labyrinth
//
//  Created by Guillaume on 12/10/13.
//
//

#ifndef __Labyrinth__Level__
#define __Labyrinth__Level__


#include "Board.h"
#include "Actor.h"
#include "Drawer.h"
#include "EventManager.h"

namespace cinder {
    namespace app {
        class App;
    }
}
namespace ci = cinder;

struct Level
{
    Board terrain;
    Actor player;
    Drawer drawer;
    EventManager eventManager;

    /// Call it before use.
    void prepare(ci::app::App * app);

    /// Call it before delete.
    void destroy(ci::app::App * app);
};

#endif
