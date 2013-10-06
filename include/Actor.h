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

#include <functional>


namespace cinder {
    class Timeline;
}


class Actor
{
public:
    Actor(ci::Timeline & timeline)
    : mTimeline(timeline),
      mNextMove(DIR_NONE),
      mTileId(0)
    {
        memset(&mDirectionToTileId, 0, sizeof(mDirectionToTileId));
    }

    void setTileId(int id, Direction dir)
    {
        assert(id>0);
        mDirectionToTileId[dir] = id;
        if(dir==DIR_NONE)
        {
            mTileId = id;
        }
    }

    /// The tile to display the actor
    int tileId() const
    {
        return mTileId;
    }

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

    /// Given function called at the start of a new move.
    void setStartMoveCallback(const std::function<void(ci::Vec2i)> & cb)
    {
        mStartMoveCb = cb;
    }

    /// Given function called at the end of the current move.
    void setFinishMoveCallback(const std::function<void(ci::Vec2i)> & cb)
    {
        mFinishMoveCb = cb;
    }

private:

    /// Animate the actor to go in the given direction
    void move(Direction d);

private:
    ci::Timeline & mTimeline;
    ci::Anim<ci::Vec2f> mAnimatedPosition;

    ci::Vec2i mPosition;
    Direction mNextMove;

    int mTileId;
    int mDirectionToTileId[DIR_MAX]; // DIR_* are not contiguous, not a big deal

    std::function<void(ci::Vec2i)> mStartMoveCb;
    std::function<void(ci::Vec2i)> mFinishMoveCb;
};

#endif
