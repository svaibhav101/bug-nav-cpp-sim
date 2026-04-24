/**
 * @file mapfile.cpp
 * @brief .map map file save and load implementation.
 */
#include "mapfile.hpp"
#include <fstream>
#include <sstream>

// ---------------------------------------------------------------------------
// .map file format (plain text, one record per line):
//   # comment line
//   S x y      - start position
//   G x y      - goal position
//   K x y      - checkpoints
//   O x y      - obstacle cell
// ---------------------------------------------------------------------------

bool saveMap(const std::string &path, const MapData &d)
{
    std::ofstream f(path);
    if (!f)
        return false;

    f << "# Bug Path Planner map\n";
    f << "# S x y  -> start position\n";
    f << "# G x y  -> goal position\n";
    f << "# K x y  -> checkpoints\n";
    f << "# O x y  -> obstacle cell\n";
    if (d.hasStart)
        f << "S " << d.startX << ' ' << d.startY << '\n';
    if (d.hasGoal)
        f << "G " << d.goalX << ' ' << d.goalY << '\n';
    for (auto [x, y] : d.checkpoints)
        f << "K " << x << ' ' << y << '\n';
    for (auto [x, y] : d.obstacles)
        f << "O " << x << ' ' << y << '\n';

    return f.good();
}

bool loadMap(const std::string &path, MapData &d)
{
    std::ifstream f(path);
    if (!f)
        return false;

    d = MapData{};
    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::istringstream ss(line);
        char tag;
        int x, y;
        if (!(ss >> tag >> x >> y))
            continue;
        switch (tag)
        {
        case 'S':
            d.hasStart = true;
            d.startX = x;
            d.startY = y;
            break;
        case 'G':
            d.hasGoal = true;
            d.goalX = x;
            d.goalY = y;
            break;
        case 'K':
            d.checkpoints.emplace_back(x, y);
            break;
        case 'O':
            d.obstacles.emplace_back(x, y);
            break;
        default:
            break;
        }
    }
    return true;
}
