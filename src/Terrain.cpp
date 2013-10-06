//
//  Terrain.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#include "Terrain.h"
#include "Constants.h"


Terrain::Terrain()
{ }

Terrain::Terrain(int width, int height)
: mWidth(width),
  mHeight(height),
  mCells(width*height)
{ }

Terrain::Cell::Cell()
: mGround(0),
  mTrigger(0)
{ }

int Terrain::Cell::groundId() const
{
    return std::abs(mGround);
}

void Terrain::Cell::setGroundId(int id, bool blocking)
{
    assert(id > 0);
    mGround = blocking ? -id : id;
}

bool Terrain::Cell::blocking() const
{
    return mGround < 0;
}

int Terrain::Cell::triggerId() const
{
    return mTrigger;
}

void Terrain::Cell::setTriggerId(int id)
{
    assert(id > 0);
    mTrigger = id;
}

unsigned char Terrain::availableMoves(int x, int y)
{
    unsigned char result = 0;
    result |= (y > 0 && !cell(x, y-1).blocking()) ? DIR_UP : 0;
    result |= (y < mHeight-1 && !cell(x, y+1).blocking()) ? DIR_DOWN : 0;
    result |= (x > 0 && !cell(x-1, y).blocking()) ? DIR_LEFT : 0;
    result |= (x < mWidth-1 && !cell(x+1, y).blocking()) ? DIR_RIGHT : 0;
    return result;
}
