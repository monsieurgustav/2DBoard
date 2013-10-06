//
//  Drawer.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 05/10/13.
//
//

#ifndef __Labyrinth__Drawer__
#define __Labyrinth__Drawer__


#include "cinder/gl/Texture.h"
#include "cinder/Tween.h"

#include <unordered_map>

class Terrain;
class Actor;

class Drawer
{
public:

    Drawer(ci::Timeline & timeline, int tileSize)
     : mTimeline(timeline), mTileSize(tileSize),
       mWindowWidth(0), mWindowHeight(0)
    { }

    /// Adjust the display to the new window size.
    void setWindowSize(int width, int height)
    {
        mWindowWidth = width;
        mWindowHeight = height;
    }

    /// Bind a tile to a tile id from terrain/actor
    void setTile(int tileId, ci::gl::TextureRef image, int height, int index)
    {
        setTile(tileId, image, height, index, index+1);
    }

    /// Bind a animated tile to a terrain id.
    void setTile(int tileId, ci::gl::TextureRef image, int height,
                 int begin, int end);
    
    void draw(const Terrain & terrain, const Actor & actor) const;

private:
    struct Tile;
    void drawTile(const Tile & tile, ci::Vec2f position,
                  ci::Vec2f offset, float scale) const;

private:
    ci::Timeline & mTimeline;
    int mTileSize;
    int mWindowWidth, mWindowHeight;

    struct Tile
    {
        ci::gl::TextureRef image;
        int height;
        int begin, end;
        ci::Anim<int> current;
    };
    std::unordered_map<int, Tile> mIdToTile;
};

#endif
