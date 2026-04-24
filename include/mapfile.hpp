/**
 * @file mapfile.hpp
 * @brief Plain-text .map map file serialisation.
 *
 * The .map format is a human-readable text file where each line encodes one
 * map element:
 * @code
 *   # comment
 *   S x y    - start position
 *   G x y    - goal position
 *   K x y    - checkpoint (may appear multiple times, order preserved)
 *   O x y    - obstacle cell
 * @endcode
 */
#pragma once
#include <string>
#include <vector>
#include <utility>

/**
 * @brief Plain-data snapshot of a complete editor scene.
 *
 * Used to serialise/deserialise maps to disk and to transfer state
 * between the editor and the simulation.
 */
struct MapData
{
    bool hasStart{false};                         ///< True when a start position has been set.
    bool hasGoal{false};                          ///< True when a goal position has been set.
    int startX{0}, startY{0};                     ///< Start cell (valid when hasStart is true).
    int goalX{0}, goalY{0};                       ///< Goal cell  (valid when hasGoal  is true).
    std::vector<std::pair<int, int>> checkpoints; ///< Ordered waypoints.
    std::vector<std::pair<int, int>> obstacles;   ///< All obstacle cells.
};

/**
 * @brief Writes a MapData snapshot to a .map text file.
 * @param path  Destination file path (created or overwritten).
 * @param d     Map data to serialise.
 * @return true on success, false if the file could not be written.
 */
bool saveMap(const std::string &path, const MapData &d);

/**
 * @brief Reads a MapData snapshot from a .map text file.
 * @param path  Source file path.
 * @param d     Output struct filled on success.
 * @return true on success, false if the file could not be opened.
 */
bool loadMap(const std::string &path, MapData &d);
