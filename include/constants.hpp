/**
 * @file constants.hpp
 * @brief Window/grid layout constants and global enumerations shared by all modules.
 */
#pragma once

/// @defgroup layout Layout Constants
/// Pixel geometry of the window and the grid.
/// @{

/// Pixel side-length of one grid cell.
inline constexpr int CELL = 18;
/// Number of grid columns.
inline constexpr int COLS = 54;
/// Number of grid rows.
inline constexpr int ROWS = 40;
/// Width of the right-hand sidebar in pixels.
inline constexpr int SIDE = 220;
/// Total window width  = grid + sidebar.
inline constexpr int WIN_W = COLS * CELL + SIDE;
/// Total window height = grid + legend/example bar.
inline constexpr int WIN_H = ROWS * CELL + 72;

/// @}

/// Visual state of a single grid cell.
enum class CellState
{
    Empty,      ///< Passable, unvisited
    Obstacle,   ///< Impassable wall
    Start,      ///< Robot start position
    Goal,       ///< Target position
    Checkpoint, ///< Intermediate waypoint
    Path,       ///< Final path painted after Done
    Visited     ///< Cell the robot has passed through
};

/// Editor draw mode selected in the sidebar.
enum class DrawMode
{
    Obstacle,  ///< Left-click places obstacle cells
    Erase,     ///< Left-click removes any cell content
    Start,     ///< Left-click sets the start position (only one allowed)
    Goal,      ///< Left-click sets the goal position (only one allowed)
    Checkpoint ///< Left-click adds an ordered waypoint
};

/// Bug algorithm variant to simulate.
enum class AlgType
{
    Bug0 = 0, ///< Greedy toward goal + left-hand wall-follow
    Bug1,     ///< Full circumnavigation, return to closest point
    Bug2      ///< M-line based wall-follow exit condition
};
