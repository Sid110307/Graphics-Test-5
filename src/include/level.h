#pragma once

#include <vector>

enum class ELEMENT
{
    EMPTY,
    WALL,
    DOOR
};

class Level
{
public:
    Level(size_t width, size_t height);

    [[nodiscard]] ELEMENT at(size_t x, size_t y) const;
    ELEMENT &at(size_t x, size_t y);

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(size_t) const;

    Level generateLevel();

private:
    size_t width = 0, height = 0;
    std::vector<ELEMENT> data;
};
