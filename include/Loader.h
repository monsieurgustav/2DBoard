//
//  Loader.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 25/09/13.
//
//

#ifndef Labyrinth_Loader_h
#define Labyrinth_Loader_h

#include "cinder/DataSource.h"

#include <exception>

namespace cinder {
    namespace app {
        class App;
    }
}

struct Level;

class BadFormatException : std::runtime_error
{
public:
    BadFormatException() : std::runtime_error("Invalid file format")
    { }
};

ci::gl::TextureRef loadTexture(ci::app::App * app,
                               const std::string & imageName);

/**
 *  @brief Read a level file.
 *  @throw BadFormatException
 *
 *  Format:
 *  [ground]
 *  width height tileSizeInPixels
 *  tileId[:triggerId] ...
 *  [player]
 *  startPosX startPosY
 *  tileIdStill tileIdUp tileIdRight tileIdDown tileIdLeft
 *  [tiles]
 *  tileId filename tileHeightInPixels beginIndex [endIndex]
 */
Level loadFrom(ci::app::App * app, ci::DataSourceRef input);

#endif
