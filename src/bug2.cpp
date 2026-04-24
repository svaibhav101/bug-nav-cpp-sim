/**
 * @file bug2.cpp
 * @defgroup bug_algorithms Bug Algorithms
 * @brief Bug2 algorithm: M-line based wall-follow exit condition.
 * @{
 *
 * The M-line is the straight line from the current segment start to the goal.
 * On hitting an obstacle the robot follows the wall and exits as soon as it
 * crosses the M-line at a point closer to the goal than the hit point.  If it
 * returns to the hit point without such a crossing the goal is unreachable.
 */
#include "robot.hpp"
#include "grid.hpp"

// -----------------------------------------------------------------------------
//  Bug 2
// M-line: straight line from segment-start to goal.
// Move toward goal; on obstacle enter left-hand wall-follow.
// Leave the wall the first time the robot lands on (or within 1 cell of)
// the M-line at a point closer to the goal than the hit point.
// If the robot completes a full loop back to the hit point without finding
// such a crossing, the goal is unreachable -> Fail.
// -----------------------------------------------------------------------------

bool Robot::stepBug2(Grid &g)
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
            hitPt = {x, y};
            bug2HitDist_ = distToTarget(x, y);
            facing = rgt(d); // left-hand rule entry
            wallSteps_ = 0;
        }
    }
    else
    {
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

        // Full-circuit failure: returned to hit point without an M-line exit.
        if (wallSteps_ > 8 && x == hitPt.first && y == hitPt.second)
        {
            state = State::Failed;
            return false;
        }

        // M-line exit: wait at least 5 steps before checking.
        bool notAtHit = !(x == hitPt.first && y == hitPt.second);
        if (wallSteps_ > 5 && notAtHit && onMLine(x, y))
        {
            float dNow = distToTarget(x, y);
            if (dNow < bug2HitDist_)
            {
                int gd = bestDir(x, y, gx, gy);
                if (!g.blocked(x + DX4[gd], y + DY4[gd]))
                {
                    state = State::Moving;
                    facing = gd;
                    wallSteps_ = 0;
                    hitPt = {-1, -1};
                    bug2HitDist_ = 0.f;
                }
            }
        }
    }
    return true;
}
