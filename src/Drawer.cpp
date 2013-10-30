//
//  Drawer.cpp
//  Labyrinth
//
//  Created by Guillaume Buisson on 05/10/13.
//
//

#include "Drawer.h"

#include "Board.h"
#include "Actor.h"
#include "Helpers.h"

#include "cinder/Timeline.h"


void Drawer::setTile(int tileId, ci::gl::TextureRef image, int height,
                     int line, int begin, int end)
{
    Tile tmp = {image, height, line, begin, end, begin};
    Tile & tile = mIdToTile[tileId] = tmp;
    // vs2010 does not support generalized initializer.
    // Tile & tile = mIdToTile[tileId] = {image, height, begin, end, begin};
    if(end > begin+1)
    {
        mTimeline->apply(&tile.current, end, MOVE_DURATION).loop();
    }
}

namespace
{
    ci::Area pickTileIndex(const ci::Area & imageSize,
                           int tileWidth, int tileHeight,
                           int tileLine, int tileIndex)
    {
        const int tileCountInRow = imageSize.getWidth() / tileWidth;
        tileIndex %= tileCountInRow;
        return ci::Area(tileIndex*tileWidth, (tileLine+1)*tileWidth-tileHeight,
                        (tileIndex+1)*tileWidth, (tileLine+1)*tileWidth);
    }
}

void Drawer::draw(const Board &terrain, const Actor &actor) const
{
    // integer scale to preserve pixels
    const int scalei = reduce_min(mWindowSize/mTileSize/mViewSize);
    if(!scalei)
    {
        return;
    }
    const float scale = static_cast<float>(scalei);

    // actual view size (depending on window ratio)
    ci::Vec2i viewSize(mViewSize, mViewSize);
    if(mWindowSize.x>mWindowSize.y)
    {
        viewSize.x = (viewSize.x * mWindowSize.x)/mWindowSize.y;
    }
    else
    {
        viewSize.y = (viewSize.y * mWindowSize.y)/mWindowSize.x;
    }

    // select a view (scroll)
    const auto viewPos = (actor.logicalPosition()-ci::Vec2i(1, 1))
                            /(viewSize-ci::Vec2i(2, 2));
    if(mCurrentView.isComplete()
       && (ci::Vec2f(viewPos)-mCurrentView).lengthSquared() > ci::EPSILON)
    {
        const auto lastViewPos = (actor.lastPosition()-ci::Vec2i(1, 1))
                                    /(viewSize-ci::Vec2i(2, 2));
        mTimeline->apply(&mCurrentView,
                         ci::Vec2f(lastViewPos), ci::Vec2f(viewPos),
                         SCROLL_DURATION);
    }


    ci::Vec2f offset((mWindowSize-viewSize*mTileSize*scale)*0.5f);
    offset -= mCurrentView.value() * ci::Vec2f(viewSize-ci::Vec2i(2, 2))
                                   * mTileSize * scale;

    const int actorY = std::ceil(actor.animatedPosition().y);
    for(int y=0; y<=actorY; ++y)
    {
        auto it=terrain.beginRow(y), end=terrain.endRow(y);
        for(int x=0; it != end; ++it, ++x)
        {
            drawTile(it->groundId(), ci::Vec2f(x, y), offset, scale);
        }
    }

    drawTile(actor.tileId(), actor.animatedPosition(), offset, scale);
    
    for(int y=actorY+1; y<terrain.height(); ++y)
    {
        auto it=terrain.beginRow(y), end=terrain.endRow(y);
        for(int x=0; it != end; ++it, ++x)
        {
            drawTile(it->groundId(), ci::Vec2f(x, y), offset, scale);
        }
    }
}

void Drawer::drawTile(int tileId, ci::Vec2f position,
                      ci::Vec2f offset, float scale) const
{
    const auto found = mIdToTile.find(tileId);
    if(found == mIdToTile.end())
    {
        return;
    }

    const Tile & tile = found->second;
    const auto imageSize = tile.image->getBounds();
    const auto src = pickTileIndex(imageSize, mTileSize, tile.height,
                                   tile.line, tile.current);
    auto dst = ci::Rectf(position+ci::Vec2f(0.f, 1.f-static_cast<float>(tile.height)/mTileSize), position+ci::Vec2f(1.f, 1.f));
    dst.scale(mTileSize*scale);
    dst.offset(offset);
    ci::gl::draw(tile.image, src, dst);
}
