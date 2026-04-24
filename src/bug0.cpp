/**
 * @file bug0.cpp
 * @defgroup bug_algorithms Bug Algorithms
 * @brief Bug0 algorithm: greedy move toward goal with left-hand wall-follow.
 * @{
 *
 * The robot heads straight for the target.  On hitting an obstacle it enters
 * left-hand wall-follow and exits only once it has made net progress past the
 * wall-follow entry point (current dist < entry dist) and a direction with
 * positive goal-progress is free with a one-step lookahead.  This prevents
 * oscillation inside concave obstacles such as U-shapes.
 */
#include "robot.hpp"
#include "grid.hpp"
#include <cmath>

// -----------------------------------------------------------------------------
//  Bug 0
// Greedy move toward goal; on collision enter left-hand wall-follow;
// resume straight-line once the robot has made net progress past the
// wall-follow entry point (current dist < entry dist) AND a non-backward
// direction with positive goal-progress is free with a one-step lookahead.
// -----------------------------------------------------------------------------

bool Robot::stepBug0(Grid &g)
{
    if (state == State::Moving)
    {
        int d = bestDir(x, y, gx, gy);
        if (!g.blocked(x + DX4[d], y + DY4[d]))
        {
            doMove(g, d);
            facing = d;
        }
        else
        {
            state = State::WallFollow;
            facing = rgt(d); // wall on left
            wallSteps_ = 0;
            hitPt = {x, y};
            bug2HitDist_ = distToTarget(x, y); // must beat this to exit
        }
    }
    else
    {
        // Left-hand rule: try lft -> straight -> rgt -> back
        bool moved = false;
        for (int t : {lft(facing), facing, rgt(facing), bak(facing)})
        {
            if (!g.blocked(x + DX4[t], y + DY4[t]))
            {
                doMove(g, t);
                facing = t;
                moved = true;
                break;
            }
        }
        if (!moved)
        {
            state = State::Failed;
            return false;
        }
        ++wallSteps_;

        // Exit wall-follow only after making net progress past the entry point.
        // Prevents oscillation inside concave obstacles (e.g. U-shapes).
        if (wallSteps_ > 3 && distToTarget(x, y) < bug2HitDist_)
        {
            int bestFree = -1;
            float bestScore = 0.0f;
            float gdx = static_cast<float>(gx - x);
            float gdy = static_cast<float>(gy - y);
            float glen = std::sqrt(gdx * gdx + gdy * gdy);
            if (glen > 0.001f)
            {
                for (int d2 = 0; d2 < 4; ++d2)
                {
                    if (d2 == bak(facing))
                        continue; // never exit backward
                    int nx = x + DX4[d2], ny = y + DY4[d2];
                    if (g.blocked(nx, ny))
                        continue;
                    float score = (DX4[d2] * gdx + DY4[d2] * gdy) / glen;
                    if (score <= bestScore)
                        continue;
                    // One-step lookahead: bestDir from candidate must be free
                    int gd2 = bestDir(nx, ny, gx, gy);
                    if (!g.blocked(nx + DX4[gd2], ny + DY4[gd2]))
                    {
                        bestScore = score;
                        bestFree = d2;
                    }
                }
            }
            if (bestFree >= 0)
            {
                state = State::Moving;
                facing = bestFree;
                wallSteps_ = 0;
            }
        }
    }
    return true;
}
