//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Core/Application.h"
#include <iostream>

Application::Application()
    : m_window(sf::VideoMode(800, 600), "ARIS Simulation"),
      m_grid(20, 15)
{
    m_window.setFramerateLimit(60);

    // Setup Grid (Walls/Shelves/ChargingStations)
    m_grid.SetCell(5, 5, CellType::Wall);
    m_grid.SetCell(10, 7, CellType::Shelf);
    m_grid.SetCell(15, 10, CellType::ChargingStation);

    // Robot(ID, StartX, StartY) spawner
    m_robots.emplace_back(1, 2, 2);
    m_robots.emplace_back(2, 4, 4);

    std::cout << "System Initialized. Robots active: " << m_robots.size() << std::endl;
}

void Application::Run() {
    while (m_window.isOpen()) {
        ProcessEvents();
        Update();
        Render();
    }
}

void Application::ProcessEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_window.close();

        // InputTests
        if (event.type == sf::Event::KeyPressed) {
            if (!m_robots.empty()) {
                if (event.key.code == sf::Keyboard::Up)    m_robots[0].TryMove(0, -1, m_grid);
                if (event.key.code == sf::Keyboard::Down)  m_robots[0].TryMove(0, 1, m_grid);
                if (event.key.code == sf::Keyboard::Left)  m_robots[0].TryMove(-1, 0, m_grid);
                if (event.key.code == sf::Keyboard::Right) m_robots[0].TryMove(1, 0, m_grid);
            }
        }
    }
}

void Application::Update() {
    // Logic updates (Robot movement) will go here later
}

void Application::Render() {
    const float elapsed = m_clock.getElapsedTime().asSeconds();
    m_window.clear();
    m_renderer.DrawGrid(m_window, m_grid, elapsed);
    m_renderer.DrawRobots(m_window, m_robots, elapsed);
    m_renderer.DrawHUD(m_window, m_robots.size(), elapsed);
    m_window.display();
}