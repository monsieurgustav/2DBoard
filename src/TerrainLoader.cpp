#include "TerrainLoader.h"

#include <fstream>
#include <sstream>
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
        int value = 0;
        int trigger = 0;

        std::string seq;
        stream >> seq;
        auto separator = seq.find(':');

        if(separator == std::string::npos)
        {
            std::istringstream parse(seq);
            parse >> value;
        }
        else
        {
            seq[separator] = ' ';
            std::istringstream parse(seq);
            parse >> value >> trigger;
        }
        if(stream.fail())
        {
            throw BadFormatException();
        }

        cell.setGroundId(std::abs(value), value<0);
        if(trigger>0)
        {
            cell.setTriggerId(trigger);
        }
    }
    return result;
}