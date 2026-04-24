/**
 * @file app_map.cpp
 * @brief Map file I/O: native file dialog (zenity), save/load, example loading,
 *        and MapData collection / application helpers.
 */
#include <cstdio>
#include <string>
#include "app.hpp"

static const char *EXAMPLE_PATHS[] = {
    "maps/vertical_wall.map",
    "maps/u_shape.map",
    "maps/double_wall.map",
    "maps/zigzag.map",
    "maps/mirror_step.map",
};

//  Native file dialog (zenity / Linux)
static std::string fileDialog(bool save)
{
    std::string cmd;
    if (save)
    {
        cmd = "zenity --file-selection --save --confirm-overwrite"
              " --title=\"Save Map\""
              " --filename=\"map.map\""
              " --file-filter=\"Bug Planner Maps | *.map\""
              " 2>/dev/null";
    }
    else
    {
        cmd = "zenity --file-selection"
              " --title=\"Open Map\""
              " --filename=\"map.map\""
              " --file-filter=\"Bug Planner Maps | *.map\""
              " 2>/dev/null";
    }

    FILE *p = popen(cmd.c_str(), "r");
    if (!p)
        return "";
    char buf[4096] = {};
    if (!fgets(buf, sizeof(buf), p))
    {
        pclose(p);
        return "";
    }
    pclose(p);

    std::string path(buf);
    while (!path.empty() && (path.back() == '\n' || path.back() == '\r'))
        path.pop_back();

    if (save && path.size() >= 4 &&
        path.substr(path.size() - 4) != ".map")
        path += ".map";

    return path;
}

//  Save / Load / Examples
void App::saveCurrentMap()
{
    std::string path = fileDialog(true);
    if (path.empty())
    {
        statusMsg_ = "Save cancelled";
        return;
    }

    MapData d = collectMapData();
    if (saveMap(path, d))
    {
        auto slash = path.rfind('/');
        statusMsg_ = "Saved: " + (slash != std::string::npos
                                      ? path.substr(slash + 1)
                                      : path);
    }
    else
    {
        statusMsg_ = "Save failed!";
    }
}

void App::loadSavedMap()
{
    std::string path = fileDialog(false);
    if (path.empty())
    {
        statusMsg_ = "Load cancelled";
        return;
    }

    MapData d;
    if (loadMap(path, d))
    {
        applyMapData(d);
        auto slash = path.rfind('/');
        statusMsg_ = "Loaded: " + (slash != std::string::npos
                                       ? path.substr(slash + 1)
                                       : path);
    }
    else
    {
        statusMsg_ = "Load failed!";
    }
}

void App::loadExample(int idx)
{
    MapData d;
    if (loadMap(EXAMPLE_PATHS[idx], d))
    {
        applyMapData(d);
        std::string name = EXAMPLE_PATHS[idx];
        auto slash = name.rfind('/');
        if (slash != std::string::npos)
            name = name.substr(slash + 1);
        statusMsg_ = "Loaded example: " + name;
    }
    else
    {
        statusMsg_ = std::string("Could not load ") + EXAMPLE_PATHS[idx];
    }
}

//  Map data collection / application
void App::applyMapData(const MapData &d)
{
    resetSim();
    grid_.clearAll();
    hasStart_ = hasGoal_ = false;
    checkpoints_.clear();

    for (auto [ox, oy] : d.obstacles)
        if (grid_.valid(ox, oy))
            grid_.set(ox, oy, CellState::Obstacle);

    if (d.hasStart && grid_.valid(d.startX, d.startY))
    {
        startX_ = d.startX;
        startY_ = d.startY;
        hasStart_ = true;
        grid_.set(startX_, startY_, CellState::Start);
    }
    if (d.hasGoal && grid_.valid(d.goalX, d.goalY))
    {
        goalX_ = d.goalX;
        goalY_ = d.goalY;
        hasGoal_ = true;
        grid_.set(goalX_, goalY_, CellState::Goal);
    }
    for (auto [cx, cy] : d.checkpoints)
        if (grid_.valid(cx, cy))
        {
            checkpoints_.emplace_back(cx, cy);
            grid_.set(cx, cy, CellState::Checkpoint);
        }
    updateActive();
}

MapData App::collectMapData() const
{
    MapData d;
    d.hasStart = hasStart_;
    d.startX = startX_;
    d.startY = startY_;
    d.hasGoal = hasGoal_;
    d.goalX = goalX_;
    d.goalY = goalY_;
    d.checkpoints = checkpoints_;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (grid_.get(c, r) == CellState::Obstacle)
                d.obstacles.emplace_back(c, r);
    return d;
}
