#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

enum class SectorType
{
    EMPTY,
    WALL,
    PLAYER_START,
    DOOR,
    DOOR_LOCKED1,
    DOOR_LOCKED2,
    DOOR_LOCKED3,
    DOOR_LOCKED4,
    SECTOR_TYPE_COUNT,
};

enum class ObjectType
{
    KEY1,
    KEY2,
    KEY3,
    KEY4,
    OBJECT_TYPE_COUNT,
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

class Object
{
public:
    explicit Object(ObjectType t = ObjectType::KEY1, float x = 0.0f, float y = 0.0f);

    [[nodiscard]] ObjectType getType() const;
    [[nodiscard]] float getX() const;
    [[nodiscard]] float getY() const;

    void setType(ObjectType t);
    void setX(float _x);
    void setY(float _y);

private:
    ObjectType type;
    float x, y;
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

    void setObject(Object object);
    void removeObject(size_t i);
    [[nodiscard]] Object getObject(size_t i) const;
    [[nodiscard]] std::vector<Object> getObjects() const;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(size_t i) const;

private:
    std::vector<std::vector<Sector>> sectors;
    std::vector<Object> objects;
};
