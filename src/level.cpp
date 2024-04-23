#include "include/level.h"

Sector::Sector(SectorType t) : type(t) {}

SectorType Sector::getType() const { return type; }
void Sector::setType(SectorType t) { type = t; }

Object::Object(ObjectType t, float x, float y) : type(t), x(x), y(y) {}

ObjectType Object::getType() const { return type; }
float Object::getX() const { return x; }
float Object::getY() const { return y; }

void Object::setType(ObjectType t) { type = t; }
void Object::setX(float _x) { x = _x; }
void Object::setY(float _y) { y = _y; }

Level::Level() : sectors(0) {}
Level::Level(std::vector<std::vector<Sector>> sectors) : sectors(std::move(sectors)) {}
Level::Level(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::vector<Sector> sectorRow;
        std::istringstream iss(line);
        std::string token;

        while (iss >> token)
        {
            if (token[0] == 'S')
            {
                token = token.substr(1);
                if (std::stoi(token) < 0 || std::stoi(token) >= static_cast<int>(SectorType::SECTOR_TYPE_COUNT))
                {
                    std::cerr << "Invalid sector type: " << token << std::endl;
                    continue;
                }

                sectorRow.emplace_back(static_cast<SectorType>(std::stoi(token)));
            } else if (token[0] == 'O')
            {
                token = token.substr(1);
                if (std::stoi(token) < 0 || std::stoi(token) >= static_cast<int>(ObjectType::OBJECT_TYPE_COUNT))
                {
                    std::cerr << "Invalid object type: " << token << std::endl;
                    continue;
                }

                sectorRow.emplace_back(SectorType::EMPTY);
                objects.emplace_back(static_cast<ObjectType>(std::stoi(token)), sectorRow.size() - 1, sectors.size());
            } else
            {
                std::cerr << "Invalid token: " << token << std::endl;
                continue;
            }
        }

        sectors.push_back(sectorRow);
    }
}

void Level::setSector(size_t x, size_t y, Sector sector) { sectors[x][y] = sector; }
Sector Level::getSector(size_t x, size_t y) const { return sectors[x][y]; }
std::vector<std::vector<Sector>> Level::getSectors() const { return sectors; }

void Level::setObject(Object object) { objects.push_back(object); }
void Level::removeObject(size_t i) { objects.erase(objects.begin() + i); }
Object Level::getObject(size_t i) const { return objects[i]; }
std::vector<Object> Level::getObjects() const { return objects; }

size_t Level::size() const { return sectors.size(); }
size_t Level::size(size_t i) const { return sectors[i].size(); }
