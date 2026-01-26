//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include "Simulation/Grid.h"
#include "Simulation/Pathfinder.h"
#include "Renderer/WorldRenderer.h"
#include "Simulation/Robot.h"
#include "Core/TaskManager.h"
#include "Database/DatabaseManager.h"
#include "UI/Dashboard.h"
#include <memory>

class Application {
public:
    Application();
    ~Application();
    void Run();

private:
    void ProcessEvents();
    void Update();
    void Render();
    void InitializeGrid();
    void HandleRobotClick(int mouseX, int mouseY);

private:
    sf::RenderWindow m_window;
    Grid m_grid;
    WorldRenderer m_renderer;
    std::vector<Robot> m_robots;
    Pathfinder m_pathfinder;
    TaskManager m_taskManager;
    std::unique_ptr<DatabaseManager> m_database;
    std::unique_ptr<Dashboard> m_dashboard;
    
    sf::Clock m_clock;
    float m_updateTimer;
    int m_nextRobotId;
};