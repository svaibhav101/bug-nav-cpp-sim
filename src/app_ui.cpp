/**
 * @file app_ui.cpp
 * @brief Sidebar and bottom-bar button construction (buildButtons), active-
 *        state synchronisation (updateActive), and click dispatch (clickButton).
 */
#include "app.hpp"

static const char *EXAMPLE_NAMES[] = {
    " Vertical Wall",
    " U Shape",
    " Double Wall",
    " Zigzag",
    " Mirror Step",
};

//  Sidebar buttons
void App::buildButtons()
{
    btns_.clear();
    const int ox = COLS * CELL + 8;
    const int W = SIDE - 16;
    const int hw = (W - 4) / 2; // half-width for paired buttons
    int y = 50;

    auto add = [&](int id, std::string lbl, SDL_Color col)
    {
        btns_.push_back({{ox, y, W, 25}, std::move(lbl), false, col, id});
        y += 30;
    };
    auto addHW = [&](int idL, std::string lblL, SDL_Color colL,
                     int idR, std::string lblR, SDL_Color colR)
    {
        btns_.push_back({{ox, y, hw, 25}, std::move(lblL), false, colL, idL});
        btns_.push_back({{ox + hw + 4, y, hw, 25}, std::move(lblR), false, colR, idR});
        y += 30;
    };
    auto gap = [&](int px)
    { y += px; };

    // Draw-mode (ids 0-4)
    add(0, " Draw Obstacle", Theme::BTN_DRAW);
    add(1, " Erase", Theme::BTN_ERASE);
    add(2, " Set Start", Theme::BTN_START);
    add(3, " Set Goal", Theme::BTN_GOAL);
    add(4, " Add Checkpoint", Theme::BTN_CHECKPOINT);
    gap(16);
    // Algorithm (ids 5-7)
    add(5, " Bug 0 [0]", Theme::BTN_ALG);
    add(6, " Bug 1 [1]", Theme::BTN_ALG);
    add(7, " Bug 2 [2]", Theme::BTN_ALG);
    gap(16);
    // Control (ids 8-10)
    add(8, " Run  [R]", Theme::BTN_RUN);
    add(9, " Reset Sim  [Esc]", Theme::BTN_RESET);
    add(10, " Clear All  [C]", Theme::BTN_CLEAR);
    gap(16);
    // Files (ids 11-12)
    addHW(11, " Save Map", Theme::BTN_SPEED,
          12, " Load Map", Theme::BTN_SPEED);
    gap(16);
    // Playback (ids 13-15)
    add(13, " Pause / Play  [Space]", Theme::BTN_RUN);
    addHW(14, "<< Prev          ", Theme::BTN_SPEED,
          15, "         Next >> ", Theme::BTN_SPEED);
    gap(16);
    // Sim speed (ids 16-18)
    add(16, " Speed: Fast   (15 ms)", Theme::BTN_SPEED);
    add(17, " Speed: Medium (60 ms)", Theme::BTN_SPEED);
    add(18, " Speed: Slow  (200 ms)", Theme::BTN_SPEED);

    {
        const int LBAR = 26;
        const int exH = 22;
        const int exY = ROWS * CELL + LBAR + 18;
        const int gap4 = 4;
        const int padX = 8;
        const int exW = (COLS * CELL - 2 * padX - 4 * gap4) / 5;
        for (int i = 0; i < 5; ++i)
            btns_.push_back({{padX + i * (exW + gap4), exY, exW, exH},
                             EXAMPLE_NAMES[i],
                             false,
                             Theme::BTN_SPEED,
                             19 + i});
    }
}

void App::updateActive()
{
    for (auto &b : btns_)
        b.active = false;
    btns_[static_cast<int>(drawMode_)].active = true;    // 0-4: draw mode
    btns_[5 + static_cast<int>(algType_)].active = true; // 5-7: algorithm
    btns_[13].active = simPaused_;                       // 13: pause/play
    if (stepDelay_ <= 25)
        btns_[16].active = true;
    else if (stepDelay_ <= 80)
        btns_[17].active = true;
    else
        btns_[18].active = true;
}

void App::clickButton(int id)
{
    switch (id)
    {
    case 0:
        drawMode_ = DrawMode::Obstacle;
        break;
    case 1:
        drawMode_ = DrawMode::Erase;
        break;
    case 2:
        drawMode_ = DrawMode::Start;
        break;
    case 3:
        drawMode_ = DrawMode::Goal;
        break;
    case 4:
        drawMode_ = DrawMode::Checkpoint;
        break;
    case 5:
        algType_ = AlgType::Bug0;
        break;
    case 6:
        algType_ = AlgType::Bug1;
        break;
    case 7:
        algType_ = AlgType::Bug2;
        break;
    case 8:
        runSim();
        return;
    case 9:
        resetSim();
        return;
    case 10:
        clearAll();
        return;
    case 11:
        saveCurrentMap();
        return;
    case 12:
        loadSavedMap();
        return;
    case 13:
        if (simRunning_ || simPaused_)
        {
            simPaused_ = !simPaused_;
            if (!simPaused_)
                lastStep_ = SDL_GetTicks();
        }
        break;
    case 14:
        prevStep();
        return;
    case 15:
        nextStep();
        return;
    case 16:
        stepDelay_ = 15;
        break;
    case 17:
        stepDelay_ = 60;
        break;
    case 18:
        stepDelay_ = 200;
        break;
    case 19:
        loadExample(0);
        return;
    case 20:
        loadExample(1);
        return;
    case 21:
        loadExample(2);
        return;
    case 22:
        loadExample(3);
        return;
    case 23:
        loadExample(4);
        return;
    }
    updateActive();
}
