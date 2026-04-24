/**
 * @file app_render.cpp
 * @brief All SDL rendering: grid cells, robot overlay, sidebar, legend bar,
 *        and text drawing.
 */
#include "app.hpp"

//  Rendering
void App::drawText(const std::string &s, int x, int y,
                   SDL_Color col, TTF_Font *f)
{
    if (s.empty())
        return;
    if (!f)
        f = fnt_;
    SDL_Surface *sur = TTF_RenderText_Blended(f, s.c_str(), col);
    if (!sur)
        return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren_, sur);
    SDL_Rect dst = {x, y, sur->w, sur->h};
    SDL_RenderCopy(ren_, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(sur);
}

void App::render()
{
    setDrawColor(ren_, Theme::BG_CLEAR);
    SDL_RenderClear(ren_);

    renderGrid();
    renderLegend(); // fills bottom panel; must precede renderSidebar
    renderRobot();
    renderSidebar(); // draws all buttons (incl. example row in bottom panel)

    SDL_RenderPresent(ren_);
}

void App::renderGrid()
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            SDL_Rect rc = {c * CELL, r * CELL, CELL, CELL};
            SDL_Color col;
            switch (grid_.get(c, r))
            {
            case CellState::Empty:
                col = Theme::GRID_EMPTY;
                break;
            case CellState::Obstacle:
                col = Theme::GRID_OBSTACLE;
                break;
            case CellState::Start:
                col = Theme::GRID_START;
                break;
            case CellState::Goal:
                col = Theme::GRID_GOAL;
                break;
            case CellState::Checkpoint:
                col = Theme::GRID_CHECKPOINT;
                break;
            case CellState::Path:
                col = Theme::GRID_PATH;
                break;
            case CellState::Visited:
                col = Theme::GRID_VISITED;
                break;
            default:
                col = Theme::GRID_EMPTY;
                break;
            }
            setDrawColor(ren_, col);
            SDL_RenderFillRect(ren_, &rc);
        }
    }

    // -- grid lines (only span the actual grid area) ---------------------------
    setDrawColor(ren_, Theme::GRID_LINE);
    for (int c = 0; c <= COLS; ++c)
        SDL_RenderDrawLine(ren_, c * CELL, 0, c * CELL, ROWS * CELL);
    for (int r = 0; r <= ROWS; ++r)
        SDL_RenderDrawLine(ren_, 0, r * CELL, COLS * CELL, r * CELL);

    // -- M-line overlay (Bug 2 only) -------------------------------------------
    if (algType_ == AlgType::Bug2 && hasStart_ && hasGoal_)
    {
        int lx0, ly0, lx1, ly1;
        if (simRunning_ || simPaused_ || simDone_)
        {
            auto [sx, sy] = robot_.mLineStart();
            auto [ex, ey] = robot_.mLineEnd();
            lx0 = sx;
            ly0 = sy;
            lx1 = ex;
            ly1 = ey;
        }
        else
        {
            lx0 = startX_;
            ly0 = startY_;
            lx1 = goalX_;
            ly1 = goalY_;
        }
        setDrawColor(ren_, Theme::MLINE_OVERLAY);
        SDL_RenderDrawLine(ren_,
                           lx0 * CELL + CELL / 2, ly0 * CELL + CELL / 2,
                           lx1 * CELL + CELL / 2, ly1 * CELL + CELL / 2);
    }
}

void App::renderRobot()
{
    if (!simRunning_ && !simPaused_ && !simDone_)
        return;

    const int rx = robot_.x, ry = robot_.y;
    SDL_Rect rr = {rx * CELL + 3, ry * CELL + 3, CELL - 6, CELL - 6};
    setDrawColor(ren_, Theme::ROBOT_BODY);
    SDL_RenderFillRect(ren_, &rr);

    if (simRunning_ || simPaused_)
    {
        const int cx = rx * CELL + CELL / 2, cy = ry * CELL + CELL / 2;
        const int ex = cx + DX4[robot_.facing] * CELL / 2;
        const int ey = cy + DY4[robot_.facing] * CELL / 2;
        setDrawColor(ren_, Theme::ROBOT_ARROW);
        SDL_RenderDrawLine(ren_, cx, cy, ex, ey);
    }

    // Bug 1 debug overlays: hit point (magenta) and closest point (teal)
    if (algType_ == AlgType::Bug1 && robot_.hitPt.first >= 0)
    {
        auto [hx, hy] = robot_.hitPt;
        SDL_Rect hr = {hx * CELL + 1, hy * CELL + 1, CELL - 2, CELL - 2};
        setDrawColor(ren_, Theme::BUG1_HIT_PT);
        SDL_RenderDrawRect(ren_, &hr);

        if (robot_.closePt.first >= 0)
        {
            auto [qx, qy] = robot_.closePt;
            SDL_Rect qr = {qx * CELL + 1, qy * CELL + 1, CELL - 2, CELL - 2};
            setDrawColor(ren_, Theme::BUG1_CLOSE_PT);
            SDL_RenderDrawRect(ren_, &qr);
        }
    }
}

void App::renderSidebar()
{
    // -- background + divider --------------------------------------------------
    SDL_Rect sb = {COLS * CELL, 0, SIDE, WIN_H};
    setDrawColor(ren_, Theme::SIDEBAR_BG);
    SDL_RenderFillRect(ren_, &sb);
    setDrawColor(ren_, Theme::SIDEBAR_BORDER);
    SDL_RenderDrawLine(ren_, COLS * CELL, 0, COLS * CELL, WIN_H);

    const int ox = COLS * CELL + 8;

    // -- title -----------------------------------------------------------------
    drawText("Bug Path Planner", ox, 8, Theme::SIDEBAR_TITLE, fnt_);

    // -- section separators ----------------------------------------------------
    auto secLabel = [&](const std::string &s, int y)
    {
        drawText(s, ox, y, Theme::SECTION_LABEL, fntS_);
    };
    secLabel("DRAW MODE", btns_[0].rect.y - 13);
    secLabel("ALGORITHM", btns_[5].rect.y - 13);
    secLabel("CONTROL", btns_[8].rect.y - 13);
    secLabel("FILES", btns_[11].rect.y - 13);
    secLabel("PLAYBACK", btns_[13].rect.y - 13);
    secLabel("SIM SPEED", btns_[16].rect.y - 13);

    // -- buttons ---------------------------------------------------------------
    for (const auto &b : btns_)
    {
        setDrawColor(ren_, b.active ? Theme::BTN_ACTIVE_FILL : b.col);
        SDL_RenderFillRect(ren_, &b.rect);
        setDrawColor(ren_, b.active ? Theme::BTN_ACTIVE_BORDER : Theme::BTN_BORDER);
        SDL_RenderDrawRect(ren_, &b.rect);
        drawText(b.label, b.rect.x + 6, b.rect.y + 5,
                 b.active ? Theme::BTN_ACTIVE_TEXT : Theme::BTN_TEXT, fntS_);
    }

    // -- info panel (anchored below btns_[18], the last sidebar button) --------
    const int INFO_TOP = btns_[18].rect.y + btns_[18].rect.h + 8;

    setDrawColor(ren_, Theme::SIDEBAR_BORDER);
    SDL_RenderDrawLine(ren_, ox - 4, INFO_TOP - 4,
                       COLS * CELL + SIDE - 8, INFO_TOP - 4);

    secLabel("INFO", INFO_TOP);

    int sy = INFO_TOP + 14;

    static const char *ALG_LABEL[] = {"Bug 0", "Bug 1", "Bug 2"};
    static const char *STATE_NAMES[] = {
        "Moving", "Wall Follow", "Circumnavigate",
        "Return Close", "Done", "Failed"};

    drawText(std::string("Alg: ") + ALG_LABEL[static_cast<int>(algType_)],
             ox, sy, Theme::TEXT_SECONDARY, fntS_);
    sy += 13;

    SDL_Color stCol = simFailed_    ? Theme::STATUS_FAILED
                      : simDone_    ? Theme::STATUS_DONE
                      : simRunning_ ? Theme::STATUS_RUNNING
                                    : Theme::STATUS_IDLE;

    if (statusMsg_.size() > 27)
    {
        std::size_t split = statusMsg_.rfind(' ', 27);
        if (split == std::string::npos)
            split = 27;
        drawText(statusMsg_.substr(0, split), ox, sy, stCol, fntS_);
        sy += 13;
        drawText(statusMsg_.substr(split + 1), ox, sy, stCol, fntS_);
    }
    else
    {
        drawText(statusMsg_, ox, sy, stCol, fntS_);
    }
    sy += 13;

    if (simRunning_ || simPaused_ || simDone_ || simFailed_)
    {
        drawText("Steps: " + std::to_string(robot_.steps), ox, sy,
                 Theme::TEXT_SECONDARY, fntS_);
        sy += 13;
        drawText(STATE_NAMES[static_cast<int>(robot_.state)], ox, sy,
                 simPaused_ ? Theme::STATUS_RUNNING : Theme::TEXT_ACCENT, fntS_);
        sy += 13;
    }

    // -- keyboard hints --------------------------------------------------------
    sy += 4;
    drawText("Space: pause | <--Arrow-->: step", ox, sy, Theme::TEXT_HINT, fntS_);
    sy += 13;
    drawText("Ctrl+S: Save   Ctrl+L: Load", ox, sy, Theme::TEXT_HINT, fntS_);
}

void App::renderLegend()
{
    static const struct
    {
        SDL_Color col;
        const char *lbl;
    } BASE[] = {
        {Theme::GRID_START, "Start"},
        {Theme::GRID_GOAL, "Goal"},
        {Theme::GRID_CHECKPOINT, "Checkpoint"},
        {Theme::GRID_OBSTACLE, "Obstacle"},
        {Theme::GRID_PATH, "Path"},
        {Theme::GRID_VISITED, "Visited"},
        {Theme::ROBOT_BODY, "Robot"},
    };
    static const struct
    {
        SDL_Color col;
        const char *lbl;
    } BUG1[] = {
        {Theme::BUG1_HIT_PT, "Hit Point"},
        {Theme::BUG1_CLOSE_PT, "Closest Pt"},
    };

    const int n = 7 + (algType_ == AlgType::Bug1 ? 2 : 0);
    const int LBAR = 26;
    const int LW = COLS * CELL;
    const int ly = ROWS * CELL;

    // Fill the entire bottom panel (before renderSidebar draws buttons on top)
    SDL_Rect panel = {0, ly, LW, WIN_H - ly};
    setDrawColor(ren_, Theme::SIDEBAR_BG);
    SDL_RenderFillRect(ren_, &panel);
    setDrawColor(ren_, Theme::SIDEBAR_BORDER);
    SDL_RenderDrawLine(ren_, 0, ly, LW, ly);               // top border
    SDL_RenderDrawLine(ren_, 0, ly + LBAR, LW, ly + LBAR); // below legend strip
    drawText("EXAMPLES", 8, ly + LBAR + 3, Theme::SECTION_LABEL, fntS_);

    // Legend items
    const int itemW = LW / n;
    const int SW = 12;
    const int iy = ly + (LBAR - SW) / 2;

    auto drawItem = [&](int i, SDL_Color c, const char *lbl)
    {
        int ix = i * itemW + 8;
        SDL_Rect sw = {ix, iy, SW, SW};
        setDrawColor(ren_, c);
        SDL_RenderFillRect(ren_, &sw);
        setDrawColor(ren_, Theme::BTN_BORDER);
        SDL_RenderDrawRect(ren_, &sw);
        drawText(lbl, ix + SW + 5, iy, Theme::TEXT_LEGEND, fntS_);
    };

    for (int i = 0; i < 7; ++i)
        drawItem(i, BASE[i].col, BASE[i].lbl);
    if (algType_ == AlgType::Bug1)
        for (int i = 0; i < 2; ++i)
            drawItem(7 + i, BUG1[i].col, BUG1[i].lbl);
}
