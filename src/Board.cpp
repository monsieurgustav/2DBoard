//
//  Board.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#include "Board.h"
#include "Constants.h"


Board::Board()
{ }

Board::Board(int width, int height)
: mWidth(width),
  mHeight(height),
  mCells(width*height)
{ }

Board::Cell::Cell()
: mGround(0),
  mTrigger(-1)
{ }

int Board::Cell::groundId() const
{
    return std::abs(mGround);
}

void Board::Cell::setGroundId(int id, bool blocking)
{
    assert(id > 0);
    mGround = blocking ? -id : id;
}

bool Board::Cell::blocking() const
{
    return mGround < 0;
}

int Board::Cell::triggerId() const
{
    return mTrigger;
}

void Board::Cell::setTriggerId(int id)
{
    assert(id > 0);
    mTrigger = id;
}

unsigned char Board::availableMoves(int x, int y)
{
    unsigned char result = 0;
    result |= (y > 0 && !cell(x, y-1).blocking()) ? DIR_UP : 0;
    result |= (y < mHeight-1 && !cell(x, y+1).blocking()) ? DIR_DOWN : 0;
    result |= (x > 0 && !cell(x-1, y).blocking()) ? DIR_LEFT : 0;
    result |= (x < mWidth-1 && !cell(x+1, y).blocking()) ? DIR_RIGHT : 0;
    return result;
}