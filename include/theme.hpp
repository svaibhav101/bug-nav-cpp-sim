/**
 * @file theme.hpp
 * @brief Centralised colour palette for the entire application.
 *
 * All SDL_Color constants used by the renderer are defined here as
 * `static const` members of the Theme namespace.  Changing a colour in
 * this file immediately affects every widget that references it.
 */
#pragma once
#include "button.hpp" // provides rgba()

/**
 * @namespace Theme
 * @brief Light gray colour palette.
 *
 * Organised into logical groups: canvas colours, grid lines, robot
 * overlays, sidebar chrome, button states, status colours and text.
 */
namespace Theme
{

    /// @defgroup theme_canvas Canvas
    static const SDL_Color BG_CLEAR = {180, 180, 180, 255};       ///< Window clear colour
    static const SDL_Color GRID_EMPTY = {220, 220, 220, 255};     ///< Passable cell
    static const SDL_Color GRID_OBSTACLE = {70, 70, 70, 255};     ///< Wall cell
    static const SDL_Color GRID_START = {34, 180, 72, 255};       ///< Start marker
    static const SDL_Color GRID_GOAL = {230, 60, 50, 255};        ///< Goal marker
    static const SDL_Color GRID_CHECKPOINT = {220, 160, 20, 255}; ///< Waypoint marker
    static const SDL_Color GRID_PATH = {50, 140, 240, 255};       ///< Final path
    static const SDL_Color GRID_VISITED = {170, 185, 210, 255};   ///< Explored cell

    /// @defgroup theme_grid Grid overlays
    static const SDL_Color GRID_LINE = {160, 160, 160, 180};    ///< Cell border lines
    static const SDL_Color MLINE_OVERLAY = {200, 140, 10, 100}; ///< Bug2 M-line preview

    /// @defgroup theme_robot Robot
    static const SDL_Color ROBOT_BODY = {255, 200, 0, 255};    ///< Robot fill
    static const SDL_Color ROBOT_ARROW = {200, 60, 0, 255};    ///< Direction indicator
    static const SDL_Color BUG1_HIT_PT = {200, 40, 160, 200};  ///< Bug1 hit-point outline
    static const SDL_Color BUG1_CLOSE_PT = {0, 180, 130, 210}; ///< Bug1 closest-point outline

    /// @defgroup theme_sidebar Sidebar chrome
    static const SDL_Color SIDEBAR_BG = {200, 200, 200, 255};     ///< Sidebar background
    static const SDL_Color SIDEBAR_BORDER = {140, 140, 140, 255}; ///< Sidebar border / dividers
    static const SDL_Color SIDEBAR_TITLE = {30, 30, 30, 255};     ///< Application title text
    static const SDL_Color SECTION_LABEL = {100, 100, 100, 255};  ///< Section header text

    /// @defgroup theme_buttons Button base colours (inactive state)
    static const SDL_Color BTN_DRAW = {185, 185, 185, 255};
    static const SDL_Color BTN_ERASE = {185, 185, 185, 255};
    static const SDL_Color BTN_START = {190, 225, 195, 255};
    static const SDL_Color BTN_GOAL = {235, 195, 192, 255};
    static const SDL_Color BTN_CHECKPOINT = {235, 215, 170, 255};
    static const SDL_Color BTN_ALG = {190, 205, 235, 255};
    static const SDL_Color BTN_RUN = {160, 215, 170, 255};
    static const SDL_Color BTN_RESET = {225, 170, 165, 255};
    static const SDL_Color BTN_CLEAR = {210, 185, 230, 255};
    static const SDL_Color BTN_SPEED = {185, 185, 185, 255};

    /// @defgroup theme_active Button active state
    static const SDL_Color BTN_ACTIVE_FILL = {60, 120, 210, 255};  ///< Active button fill
    static const SDL_Color BTN_ACTIVE_TEXT = {255, 255, 255, 255}; ///< Active button label
    static const SDL_Color BTN_ACTIVE_BORDER = {40, 90, 175, 255}; ///< Active button border

    /// @defgroup theme_btn_text Button inactive text / border
    static const SDL_Color BTN_TEXT = {45, 45, 45, 255};
    static const SDL_Color BTN_BORDER = {145, 145, 145, 200};

    /// @defgroup theme_status Status bar colours
    static const SDL_Color STATUS_IDLE = {90, 90, 90, 255};     ///< No simulation active
    static const SDL_Color STATUS_RUNNING = {160, 110, 0, 255}; ///< Simulation in progress
    static const SDL_Color STATUS_DONE = {20, 140, 50, 255};    ///< Goal reached
    static const SDL_Color STATUS_FAILED = {190, 30, 20, 255};  ///< Goal unreachable

    /// @defgroup theme_text Information text
    static const SDL_Color TEXT_PRIMARY = {30, 30, 30, 255};
    static const SDL_Color TEXT_SECONDARY = {70, 70, 70, 255};
    static const SDL_Color TEXT_ACCENT = {30, 80, 180, 255};
    static const SDL_Color TEXT_LEGEND = {55, 55, 55, 255};
    static const SDL_Color TEXT_HINT = {120, 120, 120, 255};

} // namespace Theme
