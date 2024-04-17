#include "include/level.h"

Level::Level(size_t width, size_t height) : width(width), height(height), data(width * height, ELEMENT::EMPTY) {}

ELEMENT Level::at(size_t x, size_t y) const { return data.at(y * width + x); }
ELEMENT &Level::at(size_t x, size_t y) { return data.at(y * width + x); }

size_t Level::size() const { return width; }
size_t Level::size(size_t) const { return height; }

Level Level::generateLevel()
{
    for (size_t x = 0; x < width; ++x)
    {
        at(x, 0) = ELEMENT::WALL;
        at(x, height - 1) = ELEMENT::WALL;
    }
    for (size_t y = 0; y < height; ++y)
    {
        at(0, y) = ELEMENT::WALL;
        at(width - 1, y) = ELEMENT::WALL;
    }

    at(width / 2, height / 2) = ELEMENT::DOOR;
    return *this;
}
