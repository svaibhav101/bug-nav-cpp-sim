/**
 * @file app_sim.cpp
 * @brief Simulation control: run, reset, clear, tick, step-forward, step-back.
 */
#include <string>
#include "app.hpp"

static const char *ALG_NAMES[] = {"Bug 0", "Bug 1", "Bug 2"};

//  Simulation control
void App::runSim()
{
    if (!hasStart_ || !hasGoal_)
    {
        statusMsg_ = "Set a Start and Goal first!";
        return;
    }
    resetSim();
    robot_.init(startX_, startY_, goalX_, goalY_, algType_, checkpoints_);
    simRunning_ = true;
    simPaused_ = false;
    lastStep_ = SDL_GetTicks();
    statusMsg_ = std::string("Running ") + ALG_NAMES[static_cast<int>(algType_)] + "...";
}

void App::resetSim()
{
    simRunning_ = simDone_ = simFailed_ = simPaused_ = false;
    grid_.clearSim();
    if (hasStart_)
        grid_.set(startX_, startY_, CellState::Start);
    if (hasGoal_)
        grid_.set(goalX_, goalY_, CellState::Goal);
    for (auto [cx, cy] : checkpoints_)
        grid_.set(cx, cy, CellState::Checkpoint);
    statusMsg_ = "Simulation reset";
    updateActive();
}

void App::clearAll()
{
    simRunning_ = simDone_ = simFailed_ = simPaused_ = false;
    grid_.clearAll();
    hasStart_ = hasGoal_ = false;
    checkpoints_.clear();
    statusMsg_ = "Cleared -- draw a new scene";
    updateActive();
}

void App::finishSim()
{
    simRunning_ = simPaused_ = false;
    if (robot_.state == Robot::State::Done)
    {
        simDone_ = true;
        for (auto [px, py] : robot_.path)
        {
            auto cs = grid_.get(px, py);
            if (cs == CellState::Empty || cs == CellState::Visited)
                grid_.set(px, py, CellState::Path);
        }
        statusMsg_ = "Done!  Steps: " + std::to_string(robot_.steps);
    }
    else
    {
        simFailed_ = true;
        statusMsg_ = "Failed -- unreachable.  Steps: " + std::to_string(robot_.steps);
    }
}

void App::tickSim()
{
    if (!simRunning_ || simPaused_)
        return;
    if (SDL_GetTicks() - lastStep_ < static_cast<Uint32>(stepDelay_))
        return;
    lastStep_ = SDL_GetTicks();

    if (!robot_.step(grid_))
        finishSim();
}

void App::nextStep()
{
    if (simDone_ || simFailed_)
        return;
    if (!simRunning_ && !simPaused_)
        return;

    simPaused_ = true;
    updateActive();

    if (!robot_.step(grid_))
        finishSim();
    else
        statusMsg_ = "Step " + std::to_string(robot_.steps);
}

void App::prevStep()
{
    if (!simRunning_ && !simPaused_ && !simDone_ && !simFailed_)
        return;
    if (!hasStart_ || !hasGoal_)
        return;

    int target = robot_.steps - 1;
    if (target < 0)
        target = 0;

    // Rebuild grid to pre-sim state.
    grid_.clearSim();
    if (hasStart_)
        grid_.set(startX_, startY_, CellState::Start);
    if (hasGoal_)
        grid_.set(goalX_, goalY_, CellState::Goal);
    for (auto [cx, cy] : checkpoints_)
        grid_.set(cx, cy, CellState::Checkpoint);

    // Re-init robot and silently replay target steps.
    robot_.init(startX_, startY_, goalX_, goalY_, algType_, checkpoints_);
    for (int i = 0; i < target; ++i)
        robot_.step(grid_);

    simRunning_ = true;
    simPaused_ = true;
    simDone_ = false;
    simFailed_ = false;
    updateActive();
    statusMsg_ = "Step " + std::to_string(robot_.steps);
}
