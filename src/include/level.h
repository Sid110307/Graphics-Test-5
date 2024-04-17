#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

enum class SectorType
{
    EMPTY,
    WALL,
    DOOR,
    PLAYER_START,
    SECTOR_TYPE_COUNT,
};

class Sector
{
public:
    explicit Sector(SectorType t = SectorType::EMPTY);

    [[nodiscard]] SectorType getType() const;
    void setType(SectorType t);

private:
    SectorType type;
};

class Level
{
public:
    explicit Level(std::vector<std::vector<Sector>> sectors);
    explicit Level(const std::string &filename);

    Sector &getSector(size_t x, size_t y);
    [[nodiscard]] Sector getSector(size_t x, size_t y) const;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(size_t i) const;

private:
    std::vector<std::vector<Sector>> sectors;
};
