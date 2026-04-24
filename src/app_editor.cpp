/**
 * @file app_editor.cpp
 * @brief Grid editor helpers: coordinate conversion (toGrid), cell placement
 *        (applyMode), and cell removal (eraseCell).
 */
#include <algorithm>
#include "app.hpp"

//  Editor helpers
bool App::toGrid(int sx, int sy, int &gx, int &gy) const
{
    if (sx < 0 || sx >= COLS * CELL || sy < 0 || sy >= ROWS * CELL)
        return false;
    gx = sx / CELL;
    gy = sy / CELL;
    return grid_.valid(gx, gy);
}

void App::applyMode(int gx, int gy)
{
    switch (drawMode_)
    {
    case DrawMode::Obstacle:
        if (grid_.get(gx, gy) == CellState::Empty)
            grid_.set(gx, gy, CellState::Obstacle);
        break;
    case DrawMode::Erase:
        eraseCell(gx, gy);
        break;
    case DrawMode::Start:
        if (hasStart_)
            grid_.set(startX_, startY_, CellState::Empty);
        startX_ = gx;
        startY_ = gy;
        hasStart_ = true;
        grid_.set(gx, gy, CellState::Start);
        break;
    case DrawMode::Goal:
        if (hasGoal_)
            grid_.set(goalX_, goalY_, CellState::Empty);
        goalX_ = gx;
        goalY_ = gy;
        hasGoal_ = true;
        grid_.set(gx, gy, CellState::Goal);
        break;
    case DrawMode::Checkpoint:
        if (grid_.get(gx, gy) == CellState::Empty)
        {
            checkpoints_.push_back({gx, gy});
            grid_.set(gx, gy, CellState::Checkpoint);
        }
        break;
    }
}

void App::eraseCell(int gx, int gy)
{
    switch (grid_.get(gx, gy))
    {
    case CellState::Start:
        hasStart_ = false;
        break;
    case CellState::Goal:
        hasGoal_ = false;
        break;
    case CellState::Checkpoint:
    {
        auto it = std::find(checkpoints_.begin(), checkpoints_.end(),
                            std::make_pair(gx, gy));
        if (it != checkpoints_.end())
            checkpoints_.erase(it);
        break;
    }
    default:
        break;
    }
    grid_.set(gx, gy, CellState::Empty);
}
