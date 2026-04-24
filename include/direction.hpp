/**
 * @file direction.hpp
 * @brief 4-directional movement helpers for grid navigation.
 *
 * Defines the unit-vector tables DX4 / DY4 (N, E, S, W) and a set of
 * inline helpers for rotating a direction index and for picking the
 * grid direction that best aligns with an arbitrary goal vector.
 */
#pragma once
#include <cmath>

/**
 * @defgroup direction Direction Helpers
 * @brief Cardinal-direction indices and rotation helpers.
 *
 * Direction indices follow the convention:
 *   - 0 = North (y−1)
 *   - 1 = East  (x+1)
 *   - 2 = South (y+1)
 *   - 3 = West  (x−1)
 * @{
 */

/// X-component of each cardinal direction.
inline constexpr int DX4[] = {0, 1, 0, -1};
/// Y-component of each cardinal direction.
inline constexpr int DY4[] = {-1, 0, 1, 0};

/// Returns the direction 90° counter-clockwise (left) of @p d.
inline int lft(int d) { return (d + 3) % 4; }
/// Returns the direction 90° clockwise (right) of @p d.
inline int rgt(int d) { return (d + 1) % 4; }
/// Returns the direction 180° opposite to @p d.
inline int bak(int d) { return (d + 2) % 4; }

/// @}

/**
 * @brief Returns the cardinal direction index whose unit vector best aligns
 *        with the vector from (rx, ry) toward (tx, ty).
 *
 * Uses the dot-product of each axis-aligned unit vector against the
 * normalised goal vector.  If the two points coincide the function
 * returns 0 (North) as a safe fallback.
 *
 * @param rx  Robot (source) column.
 * @param ry  Robot (source) row.
 * @param tx  Target column.
 * @param ty  Target row.
 * @return    Index 0–3 corresponding to the best cardinal direction.
 */
inline int bestDir(int rx, int ry, int tx, int ty)
{
    float dx = static_cast<float>(tx - rx);
    float dy = static_cast<float>(ty - ry);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f)
        return 0;

    int best = 0;
    float bestS = -1e9f;
    for (int d = 0; d < 4; ++d)
    {
        float s = (DX4[d] * dx + DY4[d] * dy) / len;
        if (s > bestS)
        {
            bestS = s;
            best = d;
        }
    }
    return best;
}
