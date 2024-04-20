#include "include/level.h"

Sector::Sector(SectorType t) : type(t) {}

SectorType Sector::getType() const { return type; }
void Sector::setType(SectorType t) { type = t; }

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

    std::vector<std::vector<Sector>> _sectors;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::vector<Sector> row;
        std::istringstream iss(line);
        std::string token;

        while (iss >> token)
        {
            if (std::stoi(token) < 0 || std::stoi(token) >= static_cast<int>(SectorType::SECTOR_TYPE_COUNT))
            {
                std::cerr << "Invalid sector type: " << token << std::endl;
                continue;
            }

            row.emplace_back(static_cast<SectorType>(std::stoi(token)));
        }

        _sectors.push_back(row);
    }

    std::reverse(_sectors.begin(), _sectors.end());
    sectors.resize(_sectors[0].size(), std::vector<Sector>(_sectors.size()));
    for (size_t x = 0; x < _sectors.size(); ++x)
        for (size_t y = 0; y < _sectors[x].size(); ++y) sectors[y][x] = _sectors[x][y];
}

void Level::setSector(size_t x, size_t y, Sector sector) { sectors[x][y] = sector; }
Sector Level::getSector(size_t x, size_t y) const { return sectors[x][y]; }
std::vector<std::vector<Sector>> Level::getSectors() const { return sectors; }

size_t Level::size() const { return sectors.size(); }
size_t Level::size(size_t i) const { return sectors[i].size(); }
