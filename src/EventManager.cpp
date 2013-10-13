//
//  EventManager.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 05/10/13.
//
//

#include "EventManager.h"


bool EventManager::setEvent(int trigger, const Event & event)
{
    return !mBindings.insert(std::make_pair(trigger, event)).second;
}

void EventManager::runEvent(int trigger, ci::app::App * app, Level & level) const
{
    auto it = mBindings.find(trigger);
    if(it != mBindings.end())
    {
        it->second(app, level);
    }
}
