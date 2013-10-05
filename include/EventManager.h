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


class EventManager
{
public:

    /**
     * Bind an event (as a function) to a trigger.
     *
     * Binding an event will replace any previous binding for the trigger.
     *
     * @return true the event replaced a previous event.
     */
    bool setEvent(int trigger, const std::function<void()> & event);

    /// Run the event bound to the given trigger if any was bound to it.
    void runEvent(int trigger) const;

private:
    std::unordered_map<int, std::function<void()>> mBindings;
};

#endif
