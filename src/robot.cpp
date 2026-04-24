/**
 * @file robot.cpp
 * @brief Robot public interface and shared navigation helpers (init, step,
 *        updateTarget, distToTarget, onMLine, doMove, resetWallState).
 */
#include "robot.hpp"
#include "grid.hpp"
#include <cmath>

//  Public interface
void Robot::init(int startX, int startY, int goalX, int goalY,
                 AlgType alg,
                 const std::vector<std::pair<int, int>> &checkpoints)
{
    x = startX;
    y = startY;
    finalGx = goalX;
    finalGy = goalY;
    alg_ = alg;
    checkpoints_ = checkpoints;
    cpIdx_ = 0;

    updateTarget();
    segStartX = startX;
    segStartY = startY;

    state = State::Moving;
    facing = bestDir(x, y, gx, gy);

    path.clear();
    path.push_back({x, y});

    steps = 0;
    wallSteps_ = 0;
    hitPt = closePt = {-1, -1};
    closeDist_ = 1e9f;
    circumSteps_ = 0;
    fullCircuit_ = false;
    bug2HitDist_ = 0.f;
}

bool Robot::step(Grid &grid)
{
    if (state == State::Done || state == State::Failed)
        return false;
    if (++steps > MAX_STEPS)
    {
        state = State::Failed;
        return false;
    }

    if (atTarget())
    {
        if (cpIdx_ < static_cast<int>(checkpoints_.size()))
        {
            ++cpIdx_;
            segStartX = x;
            segStartY = y;
            updateTarget();
            resetWallState();
            facing = bestDir(x, y, gx, gy);
            return true;
        }
        state = State::Done;
        return false;
    }

    switch (alg_)
    {
    case AlgType::Bug0:
        return stepBug0(grid);
    case AlgType::Bug1:
        return stepBug1(grid);
    case AlgType::Bug2:
        return stepBug2(grid);
    }
    return false;
}

//  Private helpers
void Robot::updateTarget()
{
    if (cpIdx_ < static_cast<int>(checkpoints_.size()))
    {
        gx = checkpoints_[cpIdx_].first;
        gy = checkpoints_[cpIdx_].second;
    }
    else
    {
        gx = finalGx;
        gy = finalGy;
    }
}

bool Robot::atTarget() const
{
    return x == gx && y == gy;
}

float Robot::distToTarget(int px, int py) const
{
    return std::sqrt(static_cast<float>((px - gx) * (px - gx) +
                                        (py - gy) * (py - gy)));
}

bool Robot::onMLine(int px, int py) const
{
    float dx = static_cast<float>(gx - segStartX);
    float dy = static_cast<float>(gy - segStartY);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f)
        return false;

    float cross = dx * (py - segStartY) - dy * (px - segStartX);
    float distLine = std::abs(cross) / len;
    float proj = ((px - segStartX) * dx + (py - segStartY) * dy) / (len * len);
    return distLine < 1.0f && proj > 0.0f && proj < 1.0f;
}

void Robot::doMove(Grid &g, int d)
{
    x += DX4[d];
    y += DY4[d];
    path.push_back({x, y});
    auto cs = g.get(x, y);
    if (cs == CellState::Empty || cs == CellState::Visited)
        g.set(x, y, CellState::Visited);
}

void Robot::resetWallState()
{
    hitPt = closePt = {-1, -1};
    closeDist_ = 1e9f;
    circumSteps_ = 0;
    fullCircuit_ = false;
    wallSteps_ = 0;
}
