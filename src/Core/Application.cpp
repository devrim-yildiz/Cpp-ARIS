//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Core/Application.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <iostream>

Application::Application()
    : m_window(sf::VideoMode(1200, 800), "ARIS - Smart Warehouse Simulation"),
      m_grid(30, 20),
      m_updateTimer(0.0f)
{
    m_window.setFramerateLimit(60);

    InitializeGrid();

    m_robots.emplace_back(1, 2, 2);
    m_robots.emplace_back(2, 4, 4);
    m_robots.emplace_back(3, 10, 10);

    m_database = std::make_unique<DatabaseManager>();
    if (!m_database->Initialize()) {
        std::cerr << "Warning: Database initialization failed" << std::endl;
    }

    m_dashboard = std::make_unique<Dashboard>();
    if (!m_dashboard->Initialize(m_window)) {
        std::cerr << "Warning: Dashboard initialization failed" << std::endl;
    }

    for (int i = 0; i < 3; ++i) {
        m_taskManager.GenerateRandomTask(m_grid);
    }

    m_database->LogEvent("SYSTEM", "Application initialized");
    std::cout << "System Initialized. Robots active: " << m_robots.size() << std::endl;
}

Application::~Application() {
    if (m_database) {
        for (const auto& robot : m_robots) {
            m_database->UpdateRobotStats(robot.getId(), robot.GetTotalDistance(), robot.GetTasksCompleted());
        }
        m_database->SaveGrid(m_grid);
        m_database->LogEvent("SYSTEM", "Application shutting down");
    }
}

void Application::InitializeGrid() {
    for (int x = 0; x < m_grid.GetWidth(); ++x) {
        m_grid.SetCell(x, 0, CellType::Wall);
        m_grid.SetCell(x, m_grid.GetHeight() - 1, CellType::Wall);
    }
    for (int y = 0; y < m_grid.GetHeight(); ++y) {
        m_grid.SetCell(0, y, CellType::Wall);
        m_grid.SetCell(m_grid.GetWidth() - 1, y, CellType::Wall);
    }

    for (int y = 5; y < 15; y += 3) {
        for (int x = 5; x < 25; x += 4) {
            m_grid.SetCell(x, y, CellType::Shelf);
            m_grid.SetCell(x + 1, y, CellType::Shelf);
        }
    }

    m_grid.SetCell(2, 18, CellType::ChargingStation);
    m_grid.SetCell(3, 18, CellType::ChargingStation);
    m_grid.SetCell(27, 18, CellType::ChargingStation);
    m_grid.SetCell(28, 18, CellType::ChargingStation);
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
        if (m_dashboard) {
            m_dashboard->ProcessEvent(event);
        }

        if (event.type == sf::Event::Closed)
            m_window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (!m_robots.empty()) {
                if (event.key.code == sf::Keyboard::Up)    m_robots[0].TryMove(0, -1, m_grid);
                if (event.key.code == sf::Keyboard::Down)  m_robots[0].TryMove(0, 1, m_grid);
                if (event.key.code == sf::Keyboard::Left)  m_robots[0].TryMove(-1, 0, m_grid);
                if (event.key.code == sf::Keyboard::Right) m_robots[0].TryMove(1, 0, m_grid);
            }
            
            if (event.key.code == sf::Keyboard::T) {
                m_taskManager.GenerateRandomTask(m_grid);
            }
            
            if (event.key.code == sf::Keyboard::H) {
                m_grid.ResetTraffic();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                HandleRobotClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}

void Application::HandleRobotClick(int mouseX, int mouseY) {
    int gridX = mouseX / 30;
    int gridY = mouseY / 30;

    for (const auto& robot : m_robots) {
        if (robot.getX() == gridX && robot.getY() == gridY) {
            if (m_dashboard) {
                m_dashboard->SetSelectedRobotId(robot.getId());
            }
            std::cout << "Selected Robot " << robot.getId() << std::endl;
            return;
        }
    }
}

void Application::Update() {
    float deltaTime = m_clock.restart().asSeconds();
    
    if (m_dashboard) {
        ImGui::SFML::Update(m_window, sf::seconds(deltaTime));
        m_dashboard->Update(deltaTime);

        if (m_dashboard->ShouldAddTask()) {
            m_taskManager.GenerateRandomTask(m_grid);
            m_database->LogEvent("TASK", "New random task generated");
        }

        if (m_dashboard->ShouldSpawnRobot() && m_robots.size() < 20) {
            int newId = m_robots.size() + 1;
            m_robots.emplace_back(newId, 2, 2);
            m_database->LogEvent("ROBOT", "New robot spawned: " + std::to_string(newId));
            std::cout << "Robot " << newId << " spawned" << std::endl;
        }

        if (m_dashboard->ShouldKillRobot() && !m_robots.empty()) {
            int removedId = m_robots.back().getId();
            m_database->UpdateRobotStats(m_robots.back().getId(), 
                                        m_robots.back().GetTotalDistance(), 
                                        m_robots.back().GetTasksCompleted());
            m_robots.pop_back();
            m_database->LogEvent("ROBOT", "Robot removed: " + std::to_string(removedId));
            std::cout << "Robot " << removedId << " removed" << std::endl;
        }

        m_dashboard->ResetRequests();
    }

    m_taskManager.Update(m_robots, m_grid);

    for (auto& robot : m_robots) {
        robot.Update(m_grid, m_robots, m_pathfinder);
        m_grid.IncrementTraffic(robot.getX(), robot.getY());
    }

    m_updateTimer += deltaTime;
    if (m_updateTimer >= 10.0f) {
        m_updateTimer = 0.0f;
        for (const auto& robot : m_robots) {
            m_database->UpdateRobotStats(robot.getId(), robot.GetTotalDistance(), robot.GetTasksCompleted());
        }
    }
}

void Application::Render() {
    m_window.clear();
    
    bool heatmap = m_dashboard ? m_dashboard->IsHeatmapEnabled() : false;
    m_renderer.DrawGrid(m_window, m_grid, heatmap);
    m_renderer.DrawRobots(m_window, m_robots);
    
    if (m_dashboard) {
        m_dashboard->Render(m_window, m_robots, m_taskManager, m_grid);
        ImGui::SFML::Render(m_window);
    }
    
    m_window.display();
}