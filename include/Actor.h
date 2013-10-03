//
//  Actor.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#ifndef Labyrinth_Actor_h
#define Labyrinth_Actor_h

#include "Constants.h"

#include "cinder/Tween.h"

namespace cinder {
    class Timeline;
}


class Actor
{
public:
    Actor(ci::Timeline & timeline)
    : mTimeline(timeline),
      mNextMove(DIR_NONE)
    { }

    /// Used to display the actor
    ci::Vec2f animatedPosition() const
    {
        return mAnimatedPosition;
    }

    /// Used to compute the available actions
    ci::Vec2i logicalPosition() const
    {
        return mPosition;
    }

    void setStartPosition(ci::Vec2i position)
    {
        mAnimatedPosition = mPosition = position;
    }

    /*
     * If the actor is still, move immediately in the given direction.
     * If he is moving, he will move in the direction as soon as possible.
     */
    void setNextMove(Direction d);

private:

    /// Animate the actor to go in the given direction
    void move(Direction d);

private:
    ci::Timeline & mTimeline;
    ci::Anim<ci::Vec2f> mAnimatedPosition;

    ci::Vec2i mPosition;
    Direction mNextMove;
};

#endif
