/**
 * @file button.hpp
 * @brief SDL colour helpers and the Button widget struct.
 */
#pragma once
#include <SDL2/SDL.h>
#include <string>

/**
 * @defgroup sdl_helpers SDL Convenience Helpers
 * @brief Thin wrappers around common SDL colour operations.
 * @{
 */

/**
 * @brief Constructs an SDL_Color from individual RGBA components.
 * @param r  Red   (0–255).
 * @param g  Green (0–255).
 * @param b  Blue  (0–255).
 * @param a  Alpha (0–255, default 255 = fully opaque).
 * @return   Initialised SDL_Color struct.
 */
inline SDL_Color rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
{
    return {r, g, b, a};
}

/**
 * @brief Sets the active draw colour of an SDL_Renderer.
 * @param ren  Target renderer.
 * @param c    Colour to apply (RGBA).
 */
inline void setDrawColor(SDL_Renderer *ren, SDL_Color c)
{
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
}

/// @}

/**
 * @brief A clickable sidebar or bottom-bar button.
 *
 * Buttons are owned by App::btns_ and rendered in App::renderSidebar().
 * Each button carries its own base colour (col) which is overridden by the
 * active-state palette when Button::active is true.
 */
struct Button
{
    SDL_Rect rect{};    ///< Screen-space bounding rectangle.
    std::string label;  ///< UTF-8 label rendered inside the button.
    bool active{false}; ///< True when this button represents the current state.
    SDL_Color col{};    ///< Inactive fill colour (from Theme).
    int id{0};          ///< Numeric identifier used by App::clickButton().
};
