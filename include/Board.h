//
//  Board.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#ifndef Labyrinth_Board_h
#define Labyrinth_Board_h

#include <vector>
#include <cassert>


class Board
{
public:
    class Cell
    {
    public:
        Cell();

        int triggerId() const;

        /// id > 0
        void setTriggerId(int id);

        void removeTrigger();

        int groundId() const;
        
        /// id > 0
        void setGroundId(int id, bool blocking);

        int layerId() const;
        
        /// id > 0
        void setLayerId(int id);

        bool blocking() const;

    private:
        int mGround;
        int mLayer;
        int mTrigger;
    };
    
public:
    typedef std::vector<Cell> Data;
    typedef Data::iterator iterator;
    typedef Data::const_iterator const_iterator;

public:
    Board();
    Board(int width, int height);

    int width() const
    {
        return mWidth;
    }

    int height() const
    {
        return mHeight;
    }

    ci::Vec2i size() const
    {
        return ci::Vec2i(mWidth, mHeight);
    }
    
    /*
     *  Accessors + iterators
     */
    const Cell & cell(int x, int y) const
    {
        assert(x < mWidth);
        assert(y < mHeight);
        return mCells[y*mWidth+x];
    }
    Cell & cell(int x, int y)
    {
        assert(x < mWidth);
        assert(y < mHeight);
        return mCells[y*mWidth+x];
    }

    iterator begin()
    {
        return mCells.begin();
    }
    const_iterator begin() const
    {
        return mCells.begin();
    }
    
    iterator end()
    {
        return mCells.end();
    }
    const_iterator end() const
    {
        return mCells.end();
    }

    const_iterator beginRow(int y) const
    {
        assert(y < mHeight);
        return mCells.begin() + y*mWidth;
    }
    iterator beginRow(int y)
    {
        assert(y < mHeight);
        return mCells.begin() + y*mWidth;
    }

    const_iterator endRow(int y) const
    {
        assert(y < mHeight);
        return mCells.begin() + (y+1)*mWidth;
    }
    iterator endRow(int y)
    {
        assert(y < mHeight);
        return mCells.begin() + (y+1)*mWidth;
    }

    /*
     *  Query
     */

    /// returns a bitfield of DIR_UP/DOWN/LEFT/RIGHT from the given cell
    unsigned char availableMoves(int x, int y);

private:
    int mWidth, mHeight;
    std::vector<Cell> mCells;
};

#endif
