/**
 * @file grid.hpp
 * @brief Declaration of the Grid class that stores the map cell states.
 */
#pragma once
#include "constants.hpp"
#include <vector>

/**
 * @defgroup grid Environment Grid
 * @brief Holds the CellState for every cell in the COLS × ROWS map.
 * @{
 *
 * The grid is the single source of truth for the editor, the renderer,
 * and the robot algorithms.  Out-of-bounds reads return CellState::Obstacle
 * so that algorithms can treat the map boundary as a wall without extra
 * checks.
 *
 * Implementation is split between this header (trivial inline-style) and
 * src/grid.cpp.
 */
class Grid
{
public:
    /// Constructs the grid with every cell initialised to CellState::Empty.
    Grid();

    /// @return true if (x, y) is inside the grid bounds.
    bool valid(int x, int y) const;

    /**
     * @brief Returns the state of cell (x, y).
     * @return CellState::Obstacle for any out-of-bounds coordinate.
     */
    CellState get(int x, int y) const;

    /// Sets the state of cell (x, y); silently ignored when out-of-bounds.
    void set(int x, int y, CellState s);

    /**
     * @brief Returns true if the cell is impassable.
     *
     * A cell is blocked when it is out-of-bounds or has CellState::Obstacle.
     */
    bool blocked(int x, int y) const;

    /// Clears only simulation artefacts (Visited / Path -> Empty).
    void clearSim();

    /// Resets every cell to CellState::Empty.
    void clearAll();

private:
    /// Row-major storage: cells_[row][col].
    std::vector<std::vector<CellState>> cells_;
};
