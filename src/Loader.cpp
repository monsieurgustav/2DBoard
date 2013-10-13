#include "Loader.h"
#include "Level.h"
#include "Events.h"

#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/app/App.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <boost/filesystem/path.hpp>
#include <unordered_map>


ci::gl::TextureRef nameToTexture(ci::app::App * app,
                                 const std::string & imageName,
                                 std::unordered_map<std::string,
                                                    ci::gl::TextureRef> &cache)
{
    auto found = cache.find(imageName);
    if(found == cache.end())
    {
        const auto asset = app->loadAsset(imageName);
        auto tex = ci::gl::Texture::create(loadImage(asset));
        tex->setMagFilter(GL_NEAREST);
        found = cache.insert(std::make_pair(imageName, tex)).first;
    }
    return found->second;
}

static void loadEvent(EventManager & mgr, std::istream & stream)
{
    int triggerId;
    std::string eventName;
    stream >> triggerId >> eventName;
    if(stream.fail() || triggerId < 1)
    {
        return;
    }

    std::function<void (ci::app::App *, Level &)> event;
    if(eventName == "setTrigger")
    {
        int posX, posY, newTriggerId;
        stream >> posX >> posY >> newTriggerId;
        const auto position = ci::Vec2i(posX, posY);
        event = [position, newTriggerId](ci::app::App *app, Level &level)
        {
            ev::setTrigger(level.board, position, newTriggerId);
        };
    }
    else if(eventName == "setGround")
    {
        int posX, posY, tileId;
        stream >> posX >> posY >> tileId;
        const auto position = ci::Vec2i(posX, posY);
        event = [position, tileId](ci::app::App *app, Level &level)
        {
            ev::setGround(level.board, position, std::abs(tileId), tileId<0);
        };
    }
    else if(eventName == "setPlayerTiles")
    {
        int tileStill, tileUp, tileRight, tileDown, tileLeft;
        stream >> tileStill >> tileUp >> tileRight >> tileDown >> tileLeft;
        ci::Vec4i movingTiles(tileUp, tileRight, tileDown, tileLeft);
        event = [tileStill, movingTiles](ci::app::App *app, Level &level)
        {
            ev::setPlayerTiles(level.player, tileStill, movingTiles);
        };
    }
    else if(eventName == "loadLevel")
    {
        std::string levelName;
        stream >> levelName;
        event = [levelName](ci::app::App *app, Level &level)
        {
            level.destroy(app);
            try {
                level = std::move(ev::loadLevel(app, levelName));
                level.prepare(app);
            }
            catch (BadFormatException & e) {
                level.prepare(app);
            }
        };
    }
    mgr.setEvent(triggerId, event);
}

Level loadFrom(ci::app::App * app, ci::DataSourceRef input)
{
    assert(input->isFilePath());
    std::ifstream stream(input->getFilePath().native());
    std::string section, line;

    // ground
    std::getline(stream, section, '[');
    std::getline(stream, section, ']');
    if(section != "ground")
    {
        throw BadFormatException();
    }

    int width, height, tileSize;
    stream >> width >> height >> tileSize;
    if(stream.fail() || width < 1 || height < 1)
    {
        throw BadFormatException();
    }

    Board terrain(width, height);
    // vs2010 does not support range based for loop
    // for(auto & cell : terrain)
    for(auto it=terrain.begin(), end=terrain.end(); it != end; ++it)
    {
        auto & cell = *it;

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

    // player
    Actor actor(app->timeline());
    std::getline(stream, section, '[');
    std::getline(stream, section, ']');
    if(section != "player")
    {
        throw BadFormatException();
    }
    int x, y;
    stream >> x >> y;
    actor.setStartPosition(ci::Vec2i(x, y));
    int tileStill, tileUp, tileRight, tileDown, tileLeft;
    stream >> tileStill >> tileUp >> tileRight >> tileDown >> tileLeft;
    actor.setTileId(tileStill, DIR_NONE);
    actor.setTileId(tileUp, DIR_UP);
    actor.setTileId(tileRight, DIR_RIGHT);
    actor.setTileId(tileDown, DIR_DOWN);
    actor.setTileId(tileLeft, DIR_LEFT);

    // tiles
    Drawer drawer(app->timeline(), tileSize);
    std::getline(stream, section, '[');
    std::getline(stream, section);
    if(section.compare(0, 6, "tiles]") != 0)
    {
        throw BadFormatException();
    }
    int tileId, tileHeight, beginIndex, endIndex;
    std::string imageName;
    std::unordered_map<std::string, ci::gl::TextureRef> cache;
    while(stream.peek() != '[')
    {
        std::getline(stream, line);
        std::istringstream s(line);
        s >> tileId >> imageName >> tileHeight >> beginIndex;
        if(s.fail())
        {
            throw BadFormatException();
        }
        auto tex = nameToTexture(app, imageName, cache);
        s >> endIndex;
        if(s.fail())
        {
            drawer.setTile(tileId, tex, tileHeight, beginIndex);
        }
        else
        {
            drawer.setTile(tileId, tex, tileHeight, beginIndex, endIndex);
        }
    }

    // triggers
    std::getline(stream, section, '[');
    std::getline(stream, section);
    if(section.compare(0, 9, "triggers]") != 0)
    {
        throw BadFormatException();
    }
    EventManager eventManager;
    while(!stream.eof())
    {
        std::getline(stream, line);
        std::istringstream s(line);
        loadEvent(eventManager, s);
    }

    Level result(std::move(terrain),
                 std::move(actor),
                 std::move(drawer),
                 std::move(eventManager));
    return result;
}
