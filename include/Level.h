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
    namespace audio {
        class Track;
        typedef std::shared_ptr<Track> TrackRef;
    }
    template <class T> class Anim;
}
namespace ci = cinder;


struct Level
{
    ci::DataSourceRef source;

    Board board;
    Actor player;
    Drawer drawer;
    EventManager eventManager;
    std::deque<IWidgetPtr> widgets;
    std::deque<IWidgetPtr> pendingWidgets;
    std::unordered_map<std::string, std::pair<ci::audio::TrackRef,
                                              ci::Anim<float>>> sounds;
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
