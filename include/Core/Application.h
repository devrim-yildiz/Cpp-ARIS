//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include "Simulation/Grid.h"
#include "Renderer/WorldRenderer.h"
#include "Simulation/Robot.h"
#include "UI/UIManager.h"
#include "Database/DatabaseManager.h"
#include <memory>

class Application {
public:
    Application(); // Constructor: Sets up window, initializes Grid
    void Run();    // The "Game Loop": HandleInput -> Update -> Render

private:
    void ProcessEvents(); // Keyboard/Mouse inputs
    void Update();        // Move robots, update logic
    void Render();        // clear -> worldRenderer.Draw() -> display

private:
    // The Core Systems
    sf::RenderWindow m_window;
    Grid m_grid;
    WorldRenderer m_renderer;
    std::vector<Robot> m_robots;
    UIManager m_uiManager;
    std::unique_ptr<DatabaseManager> m_database;

    // Clock for timing delta-time and FPS calculation
    sf::Clock m_clock;
    sf::Clock m_fpsTimer;
    int m_frameCount;
    float m_currentFPS;
    float m_deltaTime;
};