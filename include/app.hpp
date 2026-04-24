/**
 * @file app.hpp
 * @brief Top-level application class declaration.
 */
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <utility>

#include "constants.hpp"
#include "grid.hpp"
#include "robot.hpp"
#include "button.hpp"
#include "theme.hpp"
#include "mapfile.hpp"

/**
 * @defgroup app Application Layer
 * @brief Top-level application class.
 * @{
 * Owns the SDL window, renderer, fonts, grid and robot.
 * Responsibilities are split across several translation units:
 *
 * | File              | Responsibility                              |
 * |-------------------|---------------------------------------------|
 * | app.cpp           | SDL/TTF init, run loop, cleanup             |
 * | app_events.cpp    | Keyboard and mouse event dispatch           |
 * | app_editor.cpp    | Grid draw / erase helpers                   |
 * | app_ui.cpp        | Button build, active state, click dispatch  |
 * | app_sim.cpp       | Run / reset / tick / step-forward / back    |
 * | app_map.cpp       | File dialog, save / load, example loading   |
 * | app_render.cpp    | All SDL rendering                           |
 */
class App
{
public:
    /// Initialises SDL, creates the window/renderer and loads fonts.
    /// @return false if any SDL resource could not be created.
    bool init();

    /// Enters the main event/render loop; blocks until the window is closed.
    void run();

    /// Releases all SDL resources acquired by init().
    void cleanup();

private:
    //  SDL resources
    SDL_Window *win_ = nullptr;   ///< Application window
    SDL_Renderer *ren_ = nullptr; ///< Hardware-accelerated renderer
    TTF_Font *fnt_ = nullptr;     ///< 13 px main font
    TTF_Font *fntS_ = nullptr;    ///< 11 px small font (buttons, labels)

    //  World state
    Grid grid_;   ///< The map grid shared by editor and simulation
    Robot robot_; ///< The path-planning robot

    DrawMode drawMode_{DrawMode::Obstacle}; ///< Currently selected editor tool
    AlgType algType_{AlgType::Bug0};        ///< Currently selected algorithm

    bool hasStart_{false}, hasGoal_{false};
    int startX_{0}, startY_{0};                    ///< Start cell (valid when hasStart_ is true)
    int goalX_{0}, goalY_{0};                      ///< Goal cell (valid when hasGoal_ is true)
    std::vector<std::pair<int, int>> checkpoints_; ///< Ordered intermediate waypoints

    //  Simulation state
    bool simRunning_{false}; ///< Simulation is ticking automatically
    bool simDone_{false};    ///< Robot reached the goal
    bool simFailed_{false};  ///< Robot declared the goal unreachable
    bool simPaused_{false};  ///< Simulation is paused (manual stepping)
    Uint32 lastStep_{0};     ///< SDL tick timestamp of the last auto-step
    int stepDelay_{60};      ///< Milliseconds between automatic steps

    //  Input state
    bool mouseDown_{false}; ///< True while a mouse button is held
    int mouseBtn_{0};       ///< SDL button index of the held button

    //  UI
    std::vector<Button> btns_; ///< All sidebar and bottom-bar buttons
    std::string statusMsg_;    ///< One-line status shown in the info panel

    //  Event dispatch  (app_events.cpp)
    /// Routes an SDL event to the appropriate handler.
    void handleEvent(const SDL_Event &e);
    /// Handles a fresh mouse button press (button click or grid draw).
    void onMousePress(int mx, int my, int btn);
    /// Handles mouse drag (continuous grid draw while button is held).
    void onMouseDrag(int mx, int my, int btn);

    //  Editor  (app_editor.cpp)
    /// Converts screen pixel coordinates to grid cell coordinates.
    /// @return false if the pixel is outside the grid area.
    bool toGrid(int screenX, int screenY, int &gx, int &gy) const;
    /// Applies the current draw mode to cell (gx, gy).
    void applyMode(int gx, int gy);
    /// Removes content from cell (gx, gy) regardless of the current draw mode.
    void eraseCell(int gx, int gy);

    //  Sidebar / buttons  (app_ui.cpp)
    /// Creates all Button objects and adds them to btns_.
    void buildButtons();
    /// Refreshes the active flag on all buttons to match current state.
    void updateActive();
    /// Dispatches a button click by numeric id.
    void clickButton(int id);

    //  Simulation control  (app_sim.cpp)
    /// Validates prerequisites and starts a new simulation run.
    void runSim();
    /// Stops the simulation and restores the pre-sim grid state.
    void resetSim();
    /// Clears every cell and resets all editor state.
    void clearAll();
    /// Called when robot.step() returns false; updates simDone_/simFailed_.
    void finishSim();
    /// Advances the simulation by one step if the delay has elapsed.
    void tickSim();
    /// Advances one step and enters/stays in paused mode.
    void nextStep();
    /// Replays from init to (steps−1) to simulate going back one step.
    void prevStep();

    //  Map file I/O  (app_map.cpp)
    /// Opens a save-file dialog and writes the current map.
    void saveCurrentMap();
    /// Opens an open-file dialog and loads the chosen .map file.
    void loadSavedMap();
    /// Loads one of the built-in example maps by index (0–4).
    void loadExample(int idx);
    /// Applies a MapData snapshot to the editor (replaces current scene).
    void applyMapData(const MapData &d);
    /// Collects the current editor state into a MapData snapshot.
    MapData collectMapData() const;

    //  Rendering  (app_render.cpp)
    /// Master render call: clears, draws all layers, presents.
    void render();
    /// Draws the grid cells, grid lines, and the Bug2 M-line overlay.
    void renderGrid();
    /// Draws the robot body, direction arrow, and Bug1 debug overlays.
    void renderRobot();
    /// Draws the sidebar background, section labels, buttons, and info panel.
    void renderSidebar();
    /// Draws the legend strip and example buttons in the bottom panel.
    void renderLegend();
    /// Renders a UTF-8 string as a blended texture at (x, y).
    void drawText(const std::string &s, int x, int y,
                  SDL_Color col, TTF_Font *f = nullptr);
};
