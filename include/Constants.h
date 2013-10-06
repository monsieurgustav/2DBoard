//
//  Constants.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 03/10/13.
//
//

#ifndef Labyrinth_Constants_h
#define Labyrinth_Constants_h

enum Direction
{
    DIR_NONE  =0,
    DIR_UP    =1,
    DIR_DOWN  =2,
    DIR_LEFT  =4,
    DIR_RIGHT =8,
    DIR_MAX
};


/// How long to move to one tile.
const float MOVE_DURATION = .5f;

#endif
