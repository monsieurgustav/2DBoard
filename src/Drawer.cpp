//
//  Drawer.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 05/10/13.
//
//

#include "Drawer.h"

#include "Terrain.h"
#include "Actor.h"

#include "cinder/Timeline.h"


void Drawer::setTile(int tileId, ci::gl::TextureRef image, int height,
                     int begin, int end)
{
    Tile tmp = {image, height, begin, end, begin};
    Tile & tile = mIdToTile[tileId] = tmp;
    // vs2010 does not support generalized initializer.
    // Tile & tile = mIdToTile[tileId] = {image, height, begin, end, begin};
    if(end > begin+1)
    {
        mTimeline.apply(&tile.current, end, MOVE_DURATION).loop();
    }
}

namespace
{
    ci::Area pickTileIndex(const ci::Area & imageSize,
                           int tileWidth, int tileHeight,
                           int tileIndex)
    {
        const int tileCountInRow = imageSize.getWidth() / tileWidth;
        const int x = tileIndex % tileCountInRow;
        const int y = tileIndex / tileCountInRow;
        return ci::Area(x*tileWidth, y*tileHeight,
                        (x+1)*tileWidth, (y+1)*tileHeight);
    }
}

void Drawer::draw(const Terrain &terrain, const Actor &actor) const
{
    // integer scale to preserve pixels
    const int scalei = std::min(mWindowWidth/mTileSize/terrain.width(),
                                mWindowHeight/mTileSize/terrain.height());
    if(!scalei)
    {
        return;
    }
    const float scale(scalei);

    const ci::Vec2f offset((mWindowWidth-terrain.width()*mTileSize*scale)*0.5f,
                           (mWindowHeight-terrain.height()*mTileSize*scale)*0.5f);

    const int actorY = std::ceil(actor.animatedPosition().y);
    for(int y=0; y<=actorY; ++y)
    {
        auto it=terrain.beginRow(y), end=terrain.endRow(y);
        for(int x=0; it != end; ++it, ++x)
        {
            drawTile(mIdToTile.at(it->groundId()), ci::Vec2f(x, y),
                     offset, scale);
        }
    }
    drawTile(mIdToTile.at(actor.tileId()), actor.animatedPosition(),
             offset, scale);

    for(int y=actorY+1; y<terrain.height(); ++y)
    {
        auto it=terrain.beginRow(y), end=terrain.endRow(y);
        for(int x=0; it != end; ++it, ++x)
        {
            drawTile(mIdToTile.at(it->groundId()), ci::Vec2f(x, y),
                     offset, scale);
        }
    }
}

void Drawer::drawTile(const Drawer::Tile &tile, ci::Vec2f position,
                      ci::Vec2f offset, float scale) const
{
    const auto imageSize = tile.image->getBounds();
    const auto src = pickTileIndex(imageSize, mTileSize, tile.height,
                                   tile.current);
    auto dst = ci::Rectf(position+ci::Vec2f(0.f, 1.f-static_cast<float>(tile.height)/mTileSize), position+ci::Vec2f(1.f, 1.f));
    dst.scale(mTileSize*scale);
    dst.offset(offset);
    ci::gl::draw(tile.image, src, dst);
}
