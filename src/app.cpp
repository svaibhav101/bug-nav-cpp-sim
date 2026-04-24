/**
 * @file app.cpp
 * @brief App lifecycle: SDL/TTF initialisation, main event loop, and cleanup.
 */
#include <iostream>
#include "app.hpp"

//  Font search paths
static const char *FONT_PATHS[] = {
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
    "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
    nullptr};

//  Lifecycle
bool App::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
        return false;
    }
    if (TTF_Init() < 0)
    {
        std::cerr << "TTF_Init: " << TTF_GetError() << '\n';
        return false;
    }

    win_ = SDL_CreateWindow("Bug Path Planner  (Bug0 / Bug1 / Bug2)",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win_)
    {
        std::cerr << SDL_GetError() << '\n';
        return false;
    }

    ren_ = SDL_CreateRenderer(win_, -1,
                              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren_)
    {
        std::cerr << SDL_GetError() << '\n';
        return false;
    }
    SDL_SetRenderDrawBlendMode(ren_, SDL_BLENDMODE_BLEND);

    for (int i = 0; FONT_PATHS[i]; ++i)
    {
        fnt_ = TTF_OpenFont(FONT_PATHS[i], 13);
        fntS_ = TTF_OpenFont(FONT_PATHS[i], 11);
        if (fnt_ && fntS_)
            break;
        if (fnt_)
        {
            TTF_CloseFont(fnt_);
            fnt_ = nullptr;
        }
        if (fntS_)
        {
            TTF_CloseFont(fntS_);
            fntS_ = nullptr;
        }
    }
    if (!fnt_)
    {
        std::cerr << "Could not load any TTF font.\n";
        return false;
    }

    buildButtons();
    updateActive();
    statusMsg_ = "Ready -- draw obstacles, set start & goal, then Run";
    return true;
}

void App::run()
{
    bool quit = false;
    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            handleEvent(e);
        }
        tickSim();
        render();
    }
}

void App::cleanup()
{
    if (fnt_)
        TTF_CloseFont(fnt_);
    if (fntS_)
        TTF_CloseFont(fntS_);
    if (ren_)
        SDL_DestroyRenderer(ren_);
    if (win_)
        SDL_DestroyWindow(win_);
    TTF_Quit();
    SDL_Quit();
}
