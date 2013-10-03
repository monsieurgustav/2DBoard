//
//  Terrain.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#include "Terrain.h"

Terrain::Terrain()
{ }

Terrain::Terrain(int width, int height)
: mWidth(width),
  mHeight(height),
  mCells(width*height)
{ }

Terrain::Cell::Cell()
: mGround(0),
  mItem(-1)
{ }

int Terrain::Cell::groundId() const
{
    return mGround < 0 ? -1 : mGround;
}

void Terrain::Cell::setGroundId(int id)
{
    assert(id >= 0);
    mGround = id;
}

int Terrain::Cell::wallId() const
{
    return mGround < 0 ? (-mGround-1) : 0;
}

void Terrain::Cell::setWallId(int id)
{
    assert(id >= 0);
    mGround = -1-id;
}

int Terrain::Cell::itemId() const
{
    return mItem;
}

void Terrain::Cell::setItemId(int id)
{
    assert(id >= 0);
    mItem = id;
}
