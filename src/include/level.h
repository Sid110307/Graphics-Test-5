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

enum class SectorFlags
{
    IS_DOOR_OPEN = 1,
};

class Sector
{
public:
    explicit Sector(SectorType t = SectorType::EMPTY);

    [[nodiscard]] SectorType getType() const;
    void setType(SectorType t);

    [[nodiscard]] bool getFlag(SectorFlags flag) const;
    void setFlag(SectorFlags flag);
    void clearFlag(SectorFlags flag);

private:
    SectorType type;
    unsigned int flags;
};

class Level
{
public:
    Level();
    explicit Level(std::vector<std::vector<Sector>> sectors);
    explicit Level(const std::string &filename);

    void setSector(size_t x, size_t y, Sector sector);
    [[nodiscard]] Sector getSector(size_t x, size_t y) const;
    [[nodiscard]] std::vector<std::vector<Sector>> getSectors() const;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(size_t i) const;

private:
    std::vector<std::vector<Sector>> sectors;
};
