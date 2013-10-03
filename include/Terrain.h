//
//  Terrain.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#ifndef Labyrinth_Terrain_h
#define Labyrinth_Terrain_h

#include <vector>
#include <cassert>


class Terrain
{
public:
    class Cell
    {
    public:
        Cell();

        /// Has a pickable item on it?
        bool hasItem() const;

        int itemId() const;
        void setItemId(int id);

        int groundId() const;
        void setGroundId(int id);

        int wallId() const;
        void setWallId(int id);

    private:
        int mGround;
        int mItem;
    };

public:
    typedef std::vector<Cell> Data;
    typedef Data::iterator iterator;
    typedef Data::const_iterator const_iterator;

public:
    Terrain();
    Terrain(int width, int height);

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
