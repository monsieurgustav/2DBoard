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

class Board;
class Actor;

class Drawer
{
public:

    Drawer(ci::Timeline & timeline, int viewSize, int tileSize)
     : mTimeline(&timeline), mViewSize(viewSize), mTileSize(tileSize)
    {
        assert(viewSize > 2);
    }

    /// Adjust the display to the new window size.
    void setWindowSize(ci::Vec2i size)
    {
        mWindowSize = size;
    }

    /// Bind a tile to a tile id from terrain/actor
    void setTile(int tileId, ci::gl::TextureRef image, int height, int line, int index)
    {
        setTile(tileId, image, height, line, index, index+1);
    }

    /// Bind a animated tile to a terrain id.
    void setTile(int tileId, ci::gl::TextureRef image, int height,
                 int line, int begin, int end);
    
    void draw(const Board & terrain, const Actor & actor) const;

    bool scrolling() const
    {
        return !mScrollOffset.isComplete();
    }
    
private:
    void drawTile(int tileId, ci::Vec2f position,
                  ci::Vec2f offset, float scale) const;

private:
    ci::Timeline * mTimeline;
    int mViewSize;
    int mTileSize;
    mutable ci::Anim<ci::Vec2f> mScrollOffset;
    ci::Vec2i mWindowSize;

    struct Tile
    {
        ci::gl::TextureRef image;
        int height;
        int line, begin, end;
        ci::Anim<int> current;
    };
    std::unordered_map<int, Tile> mIdToTile;
};

#endif
