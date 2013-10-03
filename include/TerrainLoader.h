//
//  TerrainLoader.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#ifndef Labyrinth_TerrainLoader_h
#define Labyrinth_TerrainLoader_h

#include "Terrain.h"
#include "cinder/DataSource.h"
#include <exception>


class BadFormatException : std::runtime_error
{
public:
    BadFormatException() : std::runtime_error("Invalid file format")
    { }
};


Terrain loadFrom(ci::DataSourceRef input);

#endif
