#include "Loader.h"
#include "Level.h"
#include "Events.h"

#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/app/App.h"

#include "FileWatcher/FileWatcher.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <boost/filesystem/path.hpp>
#include <unordered_map>


/// loaded images collection
std::unordered_map<std::string, ci::gl::TextureRef> gCache;


namespace {
    class TextureWatch : public FW::FileWatchListener
    {
        ci::app::App * mApp;
    public:
        TextureWatch(ci::app::App * app) : mApp(app)
        { }

        virtual void handleFileAction(FW::WatchID watchid,
                                      const FW::String& dir,
                                      const FW::String& filename,
                                      FW::Action action) override
        {
            if(action != FW::Actions::Modified)
            {
                return;
            }

            // filename is:
            //  - filename.ext on Windows
            //  - absolutepath/filename.ext on MacOSX
            const auto file = boost::filesystem::path(filename).filename().string();
            auto found = gCache.find(file);
            if(found != gCache.end())
            {
                const auto asset = mApp->loadAsset(file);
                auto newTexture = ci::Surface(ci::loadImage(asset));
                found->second->update(newTexture);
            }
        }
    };
}

ci::gl::TextureRef loadTexture(ci::app::App * app,
                               const std::string & imageName)
{

    auto found = gCache.find(imageName);
    if(found == gCache.end())
    {
        const auto asset = app->loadAsset(imageName);
        auto tex = ci::gl::Texture::create(loadImage(asset));
        tex->setMagFilter(GL_NEAREST);
        found = gCache.insert(std::make_pair(imageName, tex)).first;

        // setup watcher
        static FW::FileWatcher * watcher = NULL;
        if(!watcher)
        {
            watcher = new FW::FileWatcher;
            const auto assetPath = asset->getFilePath().parent_path();
            watcher->addWatch(assetPath.string(), new TextureWatch(app));
        }
    }
    return found->second;
}


static void loadCell(Board::Cell & cell, std::istream & stream)
{
    std::string seq;
    stream >> seq;

    int layerCount = 0;
    bool hasEvent =  false;
    std::transform(seq.begin(), seq.end(),
                   seq.begin(),
                   [&layerCount, &hasEvent](char c) -> char {
                     if(c==':') {
                       hasEvent = true;
                       c = ' ';
                     } else if(c==',') {
                       if(hasEvent)  throw BadFormatException();
                       ++layerCount;
                       c = ' ';
                     }
                     return c;
                   });

    int ground = 0;
    int layer = 0;
    int trigger = 0;
    std::istringstream parse(seq);

    parse >> ground;
    if(layerCount)
    {
        parse >> layer;
    }
    if(hasEvent)
    {
        parse >> trigger;
    }

    if(stream.fail())
    {
        throw BadFormatException();
    }

    cell.setGroundId(std::abs(ground), ground<0);
    if(layer>0)
    {
        cell.setLayerId(layer);
    }
    if(trigger>0)
    {
        cell.setTriggerId(trigger);
    }
}


static EventManager::Event loadEvent(std::istream & stream)
{
    std::string eventName;
    stream >> eventName;

    std::function<void (ci::app::App *, Level &)> event;
    if(eventName == "setTrigger")
    {
        int posX, posY, newTriggerId;
        stream >> posX >> posY >> newTriggerId;
        const auto position = ci::Vec2i(posX, posY);
        return [position, newTriggerId](ci::app::App *app, Level &level)
        {
            ev::setTrigger(level.board, position, newTriggerId);
        };
    }
    else if(eventName == "removeTrigger")
    {
        int posX, posY;
        stream >> posX >> posY;
        const auto position = ci::Vec2i(posX, posY);
        return [position](ci::app::App *app, Level &level)
        {
            ev::removeTrigger(level.board, position);
        };
    }
    else if(eventName == "setGround")
    {
        int posX, posY, tileId;
        stream >> posX >> posY >> tileId;
        const auto position = ci::Vec2i(posX, posY);
        return [position, tileId](ci::app::App *app, Level &level)
        {
            ev::setGround(level.board, position, std::abs(tileId), tileId<0);
        };
    }
    else if(eventName == "setLayer")
    {
        int posX, posY, tileId;
        stream >> posX >> posY >> tileId;
        const auto position = ci::Vec2i(posX, posY);
        return [position, tileId](ci::app::App *app, Level &level)
        {
            ev::setLayer(level.board, position, tileId);
        };
    }
    else if(eventName == "removeLayer")
    {
        int posX, posY;
        stream >> posX >> posY;
        const auto position = ci::Vec2i(posX, posY);
        return [position](ci::app::App *app, Level &level)
        {
            ev::removeLayer(level.board, position);
        };
    }
    else if(eventName == "setPlayerTiles")
    {
        int tileStill, tileUp, tileRight, tileDown, tileLeft;
        stream >> tileStill >> tileUp >> tileRight >> tileDown >> tileLeft;
        ci::Vec4i movingTiles(tileUp, tileRight, tileDown, tileLeft);
        return [tileStill, movingTiles](ci::app::App *app, Level &level)
        {
            ev::setPlayerTiles(level.player, tileStill, movingTiles);
        };
    }
    else if(eventName == "loadLevel")
    {
        std::string levelName;
        stream >> levelName;
        return [levelName](ci::app::App *app, Level &level)
        {
            level.destroy(app);
            try {
                Level tmp(ev::loadLevel(app, levelName));
                level.source = app->loadAsset(levelName);
                // this erases the event manager, which holds data of this
                // event: can't do much after that call.
                level = std::move(tmp);
            }
            catch (BadFormatException &) {
                // do nothing
            }
            level.prepare(app);
        };
    }
    else if(eventName == "displayImage")
    {
        std::string imageName;
        float duration;
        stream >> imageName >> duration;
        return [imageName, duration](ci::app::App *app, Level &level)
        {
            ev::displayImage(app, level, imageName, duration);
        };
    }
    else if(eventName == "displayPrompt")
    {
        std::string imageName;
        stream >> imageName;
        return [imageName](ci::app::App *app, Level &level)
        {
            ev::displayPrompt(app, level, imageName);
        };
    }
    else if(eventName == "fadeInColor")
    {
        float duration = 0.f;
        stream >> duration;
        return [duration](ci::app::App *app, Level & level)
        {
            ev::fadeInColor(app, level, ci::Color::black(), duration);
        };
    }
    else if(eventName == "fadeOutColor")
    {
        float duration = 0.f;
        stream >> duration;
        return [duration](ci::app::App *app, Level & level)
        {
            ev::fadeOutColor(app, level, ci::Color::black(), duration);
        };
    }
    else if(eventName == "playSound")
    {
        std::string filename;
        stream >> filename;
        bool loop = false;
        stream >> loop;
        return [filename, loop](ci::app::App * app, Level & level)
        {
            ev::playSound(app, level, filename, loop);
        };
    }
    else if(eventName == "stopSound")
    {
        std::string filename;
        stream >> filename;
        return [filename](ci::app::App *, Level & level)
        {
            ev::stopSound(level, filename);
        };
    }
    else if(eventName == "fadeInSound")
    {
        std::string filename;
        float duration = 0.f;
        stream >> filename >> duration;
        return [filename, duration](ci::app::App *app, Level & level)
        {
            ev::fadeInSound(app, level, filename, duration);
        };
    }
    else if(eventName == "fadeOutSound")
    {
        std::string filename;
        float duration = 0.f;
        stream >> filename >> duration;
        return [filename, duration](ci::app::App *app, Level & level)
        {
            ev::fadeOutSound(app, level, filename, duration);
        };
    }
    else if(eventName == "wait")
    {
        float duration;
        stream >> duration;
        return [duration](ci::app::App *app, Level &level)
        {
            ev::wait(app, level, duration, false);
        };
    }
    else if(eventName == "modalWait")
    {
        float duration;
        stream >> duration;
        return [duration](ci::app::App *app, Level &level)
        {
            ev::wait(app, level, duration, true);
        };
    }
    return EventManager::Event();
}


static bool isComment(const std::string & line)
{
    return line.empty() || line[0] == '\r'
                        || line[0] == '#';
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

    int width, height, viewSize, tileSize;
    stream >> width >> height >> viewSize >> tileSize;
    if(stream.fail() || width < 1 || height < 1)
    {
        throw BadFormatException();
    }

    Board terrain(width, height);
    // vs2010 does not support range based for loop
    // for(auto & cell : terrain)
    for(auto it=terrain.begin(), end=terrain.end(); it != end; ++it)
    {
        loadCell(*it, stream);
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
    Drawer drawer(app->timeline(), viewSize, tileSize);
    std::getline(stream, section, '[');
    std::getline(stream, section);
    if(section.compare(0, 6, "tiles]") != 0)
    {
        throw BadFormatException();
    }
    int tileId, tileHeight, tileLine, beginIndex, endIndex;
    std::string imageName;
    while(stream.peek() != '[')
    {
        std::getline(stream, line);
        if(isComment(line))
        {
            continue;
        }

        std::istringstream s(line);
        s >> tileId >> imageName >> tileHeight >> tileLine >> beginIndex;
        if(s.fail())
        {
            throw BadFormatException();
        }
        auto tex = loadTexture(app, imageName);
        s >> endIndex;
        if(s.fail())
        {
            drawer.setTile(tileId, tex, tileHeight, tileLine, beginIndex);
        }
        else
        {
            float duration = 2.f * MOVE_DURATION;
            s >> duration;
            drawer.setTile(tileId, tex, tileHeight,
                           tileLine, beginIndex, endIndex, duration);
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
        if(isComment(line))
        {
            continue;
        }

        std::istringstream s(line);

        int triggerId;
        s >> triggerId;
        if(s.fail())
        {
            continue;
        }

        // look for a parenthesis (event list)
        while(s.peek() == ' ')
        {
            s.get();
        }
        if(s.peek() == '(')
        {
            s.get();  // skip '('
            std::queue<EventManager::Event> events;
            while(1)
            {
                const auto event = loadEvent(s);
                if(event)
                {
                    events.push(event);
                }
                std::getline(stream, line);
                if(line[0] == ')' || stream.eof())
                {
                    break;
                }
                if(isComment(line))
                {
                    continue;
                }
                std::istringstream other(line);
                s.swap(other);
            }
            const auto serialEvent = [events](ci::app::App *app, Level &level)
            {
                ev::serial(app, level, events);
            };
            eventManager.setEvent(triggerId, serialEvent);
        }
        else
        {
            const auto event = loadEvent(s);
            if(event)
            {
                eventManager.setEvent(triggerId, event);
            }
        }
    }

    Level result(std::move(terrain),
                 std::move(actor),
                 std::move(drawer),
                 std::move(eventManager));
    return result;
}
