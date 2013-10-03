//
//  Actor.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 03/10/13.
//
//

#include "Actor.h"

#include "cinder/Timeline.h"

namespace
{
    ci::Vec2i toDirection(ci::Vec2i p, Direction d)
    {
        if(d & DIR_UP)      return ci::Vec2i(p.x, p.y-1);
        if(d & DIR_DOWN)    return ci::Vec2i(p.x, p.y+1);
        if(d & DIR_LEFT)    return ci::Vec2i(p.x-1, p.y);
        if(d & DIR_RIGHT)   return ci::Vec2i(p.x+1, p.y);
        return p;
    }
}

void Actor::setNextMove(Direction d)
{
    if(mAnimatedPosition.isComplete())
    {
        move(d);
    }
    else
    {
        mNextMove = d;
    }
}

void Actor::move(Direction d)
{
    if(d == DIR_NONE)
    {
        return;
    }
    mPosition = toDirection(mPosition, d);
    mTimeline.apply(&mAnimatedPosition, ci::Vec2f(mPosition), 1.f)
             .finishFn([this]() { this->move(this->mNextMove); });
}