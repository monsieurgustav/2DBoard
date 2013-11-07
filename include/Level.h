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
#include "IWidget.h"

#include <memory>
#include <deque>

namespace cinder {
    namespace app {
        class App;
    }
    template <class T> class Anim;
}
namespace ci = cinder;

namespace FMOD {
    class Sound;
    class Channel;
}

struct Level
{
    ci::DataSourceRef source;

    Board board;
    Actor player;
    Drawer drawer;
    EventManager eventManager;
    std::deque<IWidgetPtr> widgets;
    std::deque<IWidgetPtr> pendingWidgets;

    struct Audio
    {
        FMOD::Sound * sound;
        FMOD::Channel * channel;
        ci::Anim<float> volume;
    };
    std::unordered_map<std::string, Audio> sounds;
    int current;

    Level(Board &&board, Actor &&player, Drawer &&drawer, EventManager &&mgr);

    /// explicit move constructor (as vs won't create it)
    Level(Level && other);

    /// stop playing sounds
    ~Level();

    /// explicit move constructor (as vs won't create it)
    Level & operator=(Level && other);

    /// Call it before use.
    void prepare(ci::app::App * app);

    /// Call it before delete.
    void destroy(ci::app::App * app);
};

typedef std::unique_ptr<Level> LevelPtr;

#endif
