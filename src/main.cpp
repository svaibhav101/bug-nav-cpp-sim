/**
 * @file main.cpp
 * @brief Application entry point.
 */
#include "app.hpp"
#include <iostream>

int main(int /*argc*/, char * /*argv*/[])
{
    App app;
    if (!app.init())
    {
        std::cerr << "Initialization failed.\n";
        return 1;
    }
    app.run();
    app.cleanup();
    return 0;
}
