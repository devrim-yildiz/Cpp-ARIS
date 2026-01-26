//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Core/Application.h"
#include <iostream>

Application::Application()
    : m_window(sf::VideoMode(1280, 800), "A.R.I.S. - Automated Robot Inventory System"),
      m_grid(20, 15),
      m_frameCount(0),
      m_currentFPS(0.0f),
      m_deltaTime(0.0f)
{
    m_window.setFramerateLimit(60);

    // Setup Grid (Walls/Shelves)
    m_grid.SetCell(5, 5, CellType::Wall);
    m_grid.SetCell(6, 5, CellType::Wall);
    m_grid.SetCell(5, 6, CellType::Wall);
    m_grid.SetCell(10, 10, CellType::Shelf);
    m_grid.SetCell(11, 10, CellType::Shelf);
    m_grid.SetCell(10, 11, CellType::Shelf);

    // Robot(ID, StartX, StartY) spawner
    m_robots.emplace_back(1, 2, 2);
    m_robots.emplace_back(2, 4, 4);
    m_robots.emplace_back(3, 6, 2);

    // Initialize database
    m_database = std::make_unique<DatabaseManager>("resources/aris_warehouse.db");
    if (m_database->Initialize()) {
        // Add initial inventory items
        m_database->AddInventoryItem("Widget-A", 150, "A-12");
        m_database->AddInventoryItem("Gadget-B", 89, "B-05");
        m_database->AddInventoryItem("Component-C", 234, "C-18");
        m_database->AddInventoryItem("Part-D", 67, "D-09");
        m_database->AddInventoryItem("Module-E", 112, "E-22");
        
        // Create some sample tasks
        m_database->CreateTask(1, "TRANSPORT", "A-12");
        m_database->CreateTask(2, "RETRIEVE", "B-05");
        
        std::cout << "Database initialized with sample data" << std::endl;
    } else {
        std::cerr << "Warning: Database initialization failed" << std::endl;
    }

    std::cout << "=== A.R.I.S. System Initialized ===" << std::endl;
    std::cout << "Robots active: " << m_robots.size() << std::endl;
    std::cout << "Grid size: " << m_grid.GetWidth() << "x" << m_grid.GetHeight() << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  Arrow Keys: Move Robot 1" << std::endl;
    std::cout << "  F1: Toggle UI Overlay" << std::endl;
    std::cout << "  ESC: Exit Application" << std::endl;
}

void Application::Run() {
    while (m_window.isOpen()) {
        m_deltaTime = m_clock.restart().asSeconds();
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

        // Handle UI events first
        m_uiManager.HandleEvent(event, m_window);

        // Keyboard input
        if (event.type == sf::Event::KeyPressed) {
            // Toggle UI with F1
            if (event.key.code == sf::Keyboard::F1) {
                m_uiManager.ToggleUI();
            }
            
            // Exit with ESC
            if (event.key.code == sf::Keyboard::Escape) {
                m_window.close();
            }
            
            // Robot control
            if (!m_robots.empty()) {
                bool moved = false;
                int oldX = m_robots[0].getX();
                int oldY = m_robots[0].getY();
                
                if (event.key.code == sf::Keyboard::Up) {
                    m_robots[0].TryMove(0, -1, m_grid);
                    moved = true;
                }
                if (event.key.code == sf::Keyboard::Down) {
                    m_robots[0].TryMove(0, 1, m_grid);
                    moved = true;
                }
                if (event.key.code == sf::Keyboard::Left) {
                    m_robots[0].TryMove(-1, 0, m_grid);
                    moved = true;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    m_robots[0].TryMove(1, 0, m_grid);
                    moved = true;
                }
                
                // Log robot movement to database
                if (moved && m_database && (oldX != m_robots[0].getX() || oldY != m_robots[0].getY())) {
                    m_database->LogRobotActivity(m_robots[0].getId(), "MOVE",
                                                m_robots[0].getX(), m_robots[0].getY());
                }
            }
        }
    }
}

void Application::Update() {
    // Calculate FPS
    m_frameCount++;
    if (m_fpsTimer.getElapsedTime().asSeconds() >= 1.0f) {
        m_currentFPS = static_cast<float>(m_frameCount) / m_fpsTimer.getElapsedTime().asSeconds();
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
    
    // Update UI
    m_uiManager.Update(m_deltaTime, m_robots, m_grid, m_currentFPS);
    
    // Logic updates (Robot movement, pathfinding, etc.) will go here later
}

void Application::Render() {
    m_window.clear(sf::Color(20, 20, 25));
    
    // Render world
    m_renderer.DrawGrid(m_window, m_grid);
    m_renderer.DrawRobots(m_window, m_robots);
    
    // Render UI overlay
    m_uiManager.Render(m_window);
    
    m_window.display();
}