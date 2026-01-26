#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "Version.hpp"
#include "Simulation/Robot.h"
#include "Core/Application.h"

static void modifyCurrentWorkingDirectory();

int main() {
    Application app;
    app.Run();
    return 0;
}

void modifyCurrentWorkingDirectory()
{
    while (!std::filesystem::exists("resources"))
    {
        std::filesystem::current_path(std::filesystem::current_path().parent_path());
    }
    auto cwd = std::filesystem::current_path();
}
