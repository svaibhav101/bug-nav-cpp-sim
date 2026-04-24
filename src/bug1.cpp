/**
 * @file bug1.cpp
 * @defgroup bug_algorithms Bug Algorithms
 * @brief Bug1 algorithm: full circumnavigation with closest-point return.
 * @{
 *
 * On hitting an obstacle the robot fully circumnavigates it while recording
 * the closest point to the goal (closePt).  After completing the circuit it
 * retraces the boundary back to closePt and resumes moving toward the goal.
 */
#include "robot.hpp"
#include "grid.hpp"

// -----------------------------------------------------------------------------
//  Bug 1
// On hitting an obstacle: fully circumnavigate it while tracking the closest
// point to the goal (closePt).  Then retrace the wall back to closePt and
// leave toward the goal.
// -----------------------------------------------------------------------------

bool Robot::stepBug1(Grid &g)
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
            state = State::Circumnavigate;
            hitPt = {x, y};
            closePt = {x, y};
            closeDist_ = distToTarget(x, y);
            facing = rgt(d);
            circumSteps_ = 0;
            fullCircuit_ = false;
        }
    }
    else if (state == State::Circumnavigate)
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
        ++circumSteps_;

        float d = distToTarget(x, y);
        if (d < closeDist_)
        {
            closeDist_ = d;
            closePt = {x, y};
        }

        // Full circuit: returned to hitPt after enough steps.
        if (circumSteps_ > 6 && x == hitPt.first && y == hitPt.second)
        {
            if (closePt == hitPt)
            {
                state = State::Failed;
                return false;
            }
            state = State::ReturnClose;
            wallSteps_ = 0;
        }
    }
    else if (state == State::ReturnClose)
    {
        if (x == closePt.first && y == closePt.second)
        {
            int gd = bestDir(x, y, gx, gy);
            if (g.blocked(x + DX4[gd], y + DY4[gd]))
            {
                state = State::Failed;
                return false;
            }
            state = State::Moving;
            facing = gd;
            return true;
        }
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
        if (++wallSteps_ > MAX_STEPS / 10)
        {
            state = State::Failed;
            return false;
        }
    }
    return true;
}
