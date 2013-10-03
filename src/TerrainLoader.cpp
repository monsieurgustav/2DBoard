#include "TerrainLoader.h"

#include <fstream>
#include <cassert>
#include <boost/filesystem/path.hpp>

Terrain loadFrom(ci::DataSourceRef input)
{
    assert(input->isFilePath());
    std::ifstream stream(input->getFilePath().native());
    int width, height;
    stream >> width >> height;
    if(stream.fail() || width < 1 || height < 1)
    {
        throw BadFormatException();
    }

    Terrain result(width, height);
    for(auto & cell : result)
    {
        int value;
        stream >> value;
        if(stream.fail())
        {
            throw BadFormatException();
        }

        if(value < 0)
        {
            cell.setWallId(-value-1);
        }
        else
        {
            cell.setGroundId(value);
        }
    }
    return result;
}