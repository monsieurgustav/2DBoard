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

ci::Vec2i Actor::lastPosition() const
{
    return toDirection(logicalPosition(), reverse(currentMove()));
}

Direction Actor::currentMove() const
{
    // animated postion goes to logical position.
    ci::Vec2f diff = ci::Vec2f(logicalPosition())-animatedPosition();
    if(diff.lengthSquared() < ci::EPSILON)
    {
        return DIR_NONE;
    }
    if(std::abs(diff.x) < ci::EPSILON)
    {
        return diff.y > 0 ? DIR_DOWN : DIR_UP;
    }
    else
    {
        return diff.x > 0 ? DIR_RIGHT : DIR_LEFT;
    }
}

void Actor::setNextMove(Direction d)
{
    mNextMove = d;
    if(mAnimatedPosition.isComplete())
    {
        move(d);
    }
}

void Actor::move(Direction d)
{
    mTileId = mDirectionToTileId[d];
    if(d == DIR_NONE)
    {
        return;
    }

    if(mStartMoveCb)
    {
        mStartMoveCb(mPosition);
    }

    mPosition = toDirection(mPosition, d);
    mTimeline->apply(&mAnimatedPosition, ci::Vec2f(mPosition), MOVE_DURATION)
              .finishFn([this]()
                       {
                           if(mFinishMoveCb)
                           {
                               mFinishMoveCb(mPosition);
                           }
                           move(mNextMove);
                       });
}
