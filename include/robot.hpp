/**
 * @file robot.hpp
 * @defgroup robot Robot System
 * @brief Declaration of the Robot class that executes Bug0 / Bug1 / Bug2.
 * @{
 *
 * The Robot owns all navigation state.  The caller initialises it once
 * with Robot::init() and then calls Robot::step() each simulation tick
 * until it returns false (Done or Failed).
 *
 * Per-algorithm logic lives in separate translation units:
 *   - src/bug0.cpp - Robot::stepBug0()
 *   - src/bug1.cpp - Robot::stepBug1()
 *   - src/bug2.cpp - Robot::stepBug2()
 */

#pragma once
#include <vector>
#include <utility>

#include "constants.hpp"
#include "direction.hpp"

class Grid; ///< Forward declaration to avoid circular includes.

/**
 * @brief Step-by-step Bug algorithm executor on a Grid.
 *
 * Supports optional ordered checkpoints: the robot navigates to each
 * checkpoint in sequence before proceeding to the final goal.
 */
class Robot
{
public:
    /**
     * @brief High-level navigation state machine state.
     */
    enum class State
    {
        Moving,         ///< Travelling directly toward the current target.
        WallFollow,     ///< Following an obstacle boundary (Bug0 / Bug2).
        Circumnavigate, ///< Full circumnavigation of an obstacle (Bug1).
        ReturnClose,    ///< Retracing wall back to closest point (Bug1).
        Done,           ///< Goal (or final checkpoint) reached.
        Failed          ///< Goal declared unreachable (step limit or full loop).
    };

    int x{0}, y{0};             ///< Current grid cell (column, row).
    int facing{0};              ///< Current direction index (0–3, see direction.hpp).
    State state{State::Moving}; ///< Current navigation state.
    int steps{0};               ///< Total steps taken since init().

    /// Bug1: cell where the robot first hit an obstacle (magenta overlay).
    std::pair<int, int> hitPt{-1, -1};
    /// Bug1: cell closest to the goal found during circumnavigation (teal overlay).
    std::pair<int, int> closePt{-1, -1};

    /// Complete path recorded during the run; painted on the grid when Done.
    std::vector<std::pair<int, int>> path;

    /**
     * @brief Initialises the robot for a new simulation run.
     *
     * Resets all state, sets start/goal, selects the algorithm and loads
     * checkpoints.  Must be called before the first step().
     *
     * @param startX      Starting column.
     * @param startY      Starting row.
     * @param goalX       Final goal column.
     * @param goalY       Final goal row.
     * @param alg         Algorithm to use.
     * @param checkpoints Ordered list of intermediate waypoints (may be empty).
     */
    void init(int startX, int startY, int goalX, int goalY,
              AlgType alg,
              const std::vector<std::pair<int, int>> &checkpoints);

    /**
     * @brief Advances the simulation by one step.
     * @param grid  The current map (modified in-place: cells marked Visited).
     * @return true  if the robot is still navigating;
     *         false if the state is Done or Failed.
     */
    bool step(Grid &grid);

    /**
     * @brief Returns the start of the current M-line segment (Bug2 overlay).
     * @return Grid cell coordinates of the segment start.
     */
    std::pair<int, int> mLineStart() const { return {segStartX, segStartY}; }

    /**
     * @brief Returns the end of the current M-line segment (Bug2 overlay).
     * @return Grid cell coordinates of the current target.
     */
    std::pair<int, int> mLineEnd() const { return {gx, gy}; }

private:
    int gx{0}, gy{0};               ///< Current segment target.
    int finalGx{0}, finalGy{0};     ///< Ultimate goal (after all checkpoints).
    int segStartX{0}, segStartY{0}; ///< Start of the current M-line segment (Bug2).

    AlgType alg_{AlgType::Bug0}; ///< Selected algorithm.

    float closeDist_{1e9f};   ///< Closest distance to goal seen during circumnavigation.
    int circumSteps_{0};      ///< Steps taken during Bug1 circumnavigation.
    bool fullCircuit_{false}; ///< True once the full circuit is completed (Bug1).
    float bug2HitDist_{0.f};  ///< Distance to target at wall-follow entry (Bug0/Bug2).
    int wallSteps_{0};        ///< Steps taken during the current wall-follow phase.

    std::vector<std::pair<int, int>> checkpoints_; ///< Ordered waypoints.
    int cpIdx_{0};                                 ///< Index of the next checkpoint.

    /// Hard cap on the number of steps before declaring failure.
    static constexpr int MAX_STEPS = 100'000;

    /// Sets gx/gy to the next checkpoint or the final goal.
    void updateTarget();
    /// Returns true when the robot is at the current target cell.
    bool atTarget() const;
    /// Euclidean distance from (px, py) to the current target.
    float distToTarget(int px, int py) const;
    /// Returns true when (px, py) lies on the current M-line segment.
    bool onMLine(int px, int py) const;
    /// Moves the robot one step in direction d, updates path and grid.
    void doMove(Grid &g, int d);
    /// Resets all wall-follow / circumnavigation tracking variables.
    void resetWallState();

    bool stepBug0(Grid &g); ///< One step of the Bug0 algorithm.
    bool stepBug1(Grid &g); ///< One step of the Bug1 algorithm.
    bool stepBug2(Grid &g); ///< One step of the Bug2 algorithm.
};

/** @} */