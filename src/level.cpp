#include "include/level.h"

Sector::Sector(SectorType t) : type(t) {}

SectorType Sector::getType() const { return type; }
void Sector::setType(SectorType t) { type = t; }

Level::Level(std::vector<std::vector<std::unique_ptr<Sector>>> sectors)
    : sectors(std::move(sectors)), width(this->sectors.size()), height(this->sectors[0].size()) {}

Level::Level(size_t width, size_t height) : width(width), height(height)
{
    sectors.resize(width);
    for (size_t x = 0; x < width; ++x)
    {
        sectors[x].resize(height);
        for (size_t y = 0; y < height; ++y) sectors[x][y] = std::make_unique<Sector>();
    }
}

Sector &Level::getSector(size_t x, size_t y) { return *sectors[x][y]; }
const Sector &Level::getSector(size_t x, size_t y) const { return *sectors[x][y]; }

size_t Level::getWidth() const { return width; }
size_t Level::getHeight() const { return height; }

size_t Level::size() const { return sectors.size(); }
size_t Level::size(size_t i) const { return sectors[i].size(); }
