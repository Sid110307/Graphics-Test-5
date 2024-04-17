#pragma once

#include <vector>
#include <memory>

enum class SectorType
{
    EMPTY,
    WALL,
    DOOR
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
    explicit Level(std::vector<std::vector<std::unique_ptr<Sector>>> sectors);
    Level(size_t width, size_t height);

    Sector &getSector(size_t x, size_t y);
    [[nodiscard]] const Sector &getSector(size_t x, size_t y) const;

    [[nodiscard]] size_t getWidth() const;
    [[nodiscard]] size_t getHeight() const;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(size_t i) const;

private:
    std::vector<std::vector<std::unique_ptr<Sector>>> sectors;
    size_t width, height;
};
