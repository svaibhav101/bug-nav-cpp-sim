/**
 * @file grid.cpp
 * @brief Grid class implementation - cell access, bounds checking, and bulk clears.
 */
#include "grid.hpp"

Grid::Grid()
    : cells_(ROWS, std::vector<CellState>(COLS, CellState::Empty))
{
}

bool Grid::valid(int x, int y) const
{
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

CellState Grid::get(int x, int y) const
{
    return valid(x, y) ? cells_[y][x] : CellState::Obstacle;
}

void Grid::set(int x, int y, CellState s)
{
    if (valid(x, y))
        cells_[y][x] = s;
}

bool Grid::blocked(int x, int y) const
{
    return !valid(x, y) || cells_[y][x] == CellState::Obstacle;
}

void Grid::clearSim()
{
    for (auto &row : cells_)
        for (auto &c : row)
            if (c == CellState::Visited || c == CellState::Path)
                c = CellState::Empty;
}

void Grid::clearAll()
{
    for (auto &row : cells_)
        for (auto &c : row)
            c = CellState::Empty;
}
