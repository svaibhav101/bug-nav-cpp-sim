/**
 * @file app_events.cpp
 * @brief SDL keyboard and mouse event dispatch (handleEvent, onMousePress,
 *        onMouseDrag).
 */
#include "app.hpp"

//  Event handling
void App::handleEvent(const SDL_Event &e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        mouseDown_ = true;
        mouseBtn_ = e.button.button;
        onMousePress(e.button.x, e.button.y, mouseBtn_);
    }
    if (e.type == SDL_MOUSEBUTTONUP)
    {
        mouseDown_ = false;
    }
    if (e.type == SDL_MOUSEMOTION && mouseDown_)
    {
        onMouseDrag(e.motion.x, e.motion.y, mouseBtn_);
    }
    if (e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_r:
            runSim();
            break;
        case SDLK_ESCAPE:
            resetSim();
            break;
        case SDLK_c:
            clearAll();
            break;
        case SDLK_SPACE:
            if (simRunning_ || simPaused_)
            {
                simPaused_ = !simPaused_;
                if (!simPaused_)
                    lastStep_ = SDL_GetTicks();
                updateActive();
            }
            break;
        case SDLK_LEFT:
            prevStep();
            break;
        case SDLK_RIGHT:
            nextStep();
            break;
        case SDLK_s:
            if (SDL_GetModState() & KMOD_CTRL)
                saveCurrentMap();
            break;
        case SDLK_l:
            if (SDL_GetModState() & KMOD_CTRL)
                loadSavedMap();
            break;
        case SDLK_0:
            algType_ = AlgType::Bug0;
            updateActive();
            break;
        case SDLK_1:
            algType_ = AlgType::Bug1;
            updateActive();
            break;
        case SDLK_2:
            algType_ = AlgType::Bug2;
            updateActive();
            break;
        default:
            break;
        }
    }
}

void App::onMousePress(int mx, int my, int btn)
{
    // Check all buttons first (sidebar + bottom bar)
    SDL_Point p = {mx, my};
    for (auto &b : btns_)
        if (SDL_PointInRect(&p, &b.rect))
        {
            clickButton(b.id);
            return;
        }

    // Grid drawing (only within the grid area)
    int gx, gy;
    if (toGrid(mx, my, gx, gy))
    {
        if (btn == 1)
            applyMode(gx, gy);
        else if (btn == 3)
            eraseCell(gx, gy);
    }
}

void App::onMouseDrag(int mx, int my, int btn)
{
    if (mx >= COLS * CELL)
        return;
    int gx, gy;
    if (toGrid(mx, my, gx, gy))
    {
        if (btn == 1)
            applyMode(gx, gy);
        else if (btn == 3)
            eraseCell(gx, gy);
    }
}
