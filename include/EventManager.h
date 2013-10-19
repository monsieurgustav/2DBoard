//
//  EventManager.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 05/10/13.
//
//

#ifndef Labyrinth_EventManager_h
#define Labyrinth_EventManager_h


#include <unordered_map>
#include <functional>

struct Level;


class EventManager
{
public:
    typedef std::function<void (ci::app::App *, Level &)> Event;

    /**
     * Bind an event (as a function) to a trigger.
     *
     * Binding an event will replace any previous binding for the trigger.
     *
     * @param trigger 0 is called when the level is loaded.
     * @return true the event replaced a previous event.
     */
    bool setEvent(int trigger, const Event & event);

    /// Run the event bound to the given trigger if any was bound to it.
    void runEvent(int trigger, ci::app::App * app, Level & level) const;

private:
    std::unordered_map<int, Event> mBindings;
};

#endif
