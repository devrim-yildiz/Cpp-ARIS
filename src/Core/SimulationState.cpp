//
// SimulationState.cpp - Grid and robot simulation with camera controls and grid editor
//
#include "Core/SimulationState.h"
#include "Core/Application.h"
#include "Core/MainMenuState.h"
#include "Simulation/Pathfinder.h"
#include <iostream>

SimulationState::SimulationState(Application& app)
    : State(app), m_grid(20, 15)
{
    if (!m_font.loadFromFile("resources/FiraCode-Regular.ttf")) {
        std::cerr << "Failed to load FiraCode-Regular.ttf" << std::endl;
    }

    // Set up a view that frames the grid with some padding
    float cellSize   = 30.0f;
    float gridW      = m_grid.GetWidth() * cellSize;
    float gridH      = m_grid.GetHeight() * cellSize;
    float padding     = cellSize * 2.0f;
    float viewWidth  = gridW + padding * 2.0f;
    float viewHeight = gridH + padding * 2.0f;

    m_view.setSize(viewWidth, viewHeight);
    m_view.setCenter(gridW / 2.0f, gridH / 2.0f);

    // Setup Grid (Walls/Shelves/ChargingStations)
    for (int x = 0; x < m_grid.GetWidth(); ++x) {
        m_grid.SetCell(x, 0, CellType::Wall);
        m_grid.SetCell(x, m_grid.GetHeight() - 1, CellType::Wall);
    }
    for (int y = 0; y < m_grid.GetHeight(); ++y) {
        m_grid.SetCell(0, y, CellType::Wall);
        m_grid.SetCell(m_grid.GetWidth() - 1, y, CellType::Wall);
    }

    // Interior walls
    m_grid.SetCell(5, 5, CellType::Wall);
    m_grid.SetCell(5, 6, CellType::Wall);
    m_grid.SetCell(5, 7, CellType::Wall);

    // Shelves
    m_grid.SetCell(10, 3, CellType::Shelf);
    m_grid.SetCell(10, 4, CellType::Shelf);
    m_grid.SetCell(10, 5, CellType::Shelf);
    m_grid.SetCell(14, 8, CellType::Shelf);
    m_grid.SetCell(14, 9, CellType::Shelf);
    m_grid.SetCell(14, 10, CellType::Shelf);

    // Charging stations
    m_grid.SetCell(1, 1, CellType::ChargingStation);
    m_grid.SetCell(18, 13, CellType::ChargingStation);

    // Spawn robots
    m_robots.emplace_back(1, 2, 2);
    m_robots.emplace_back(2, 4, 4);
    m_robots.emplace_back(3, 8, 10);

    // Apply initial letterbox view
    sf::Vector2u winSize = m_app.GetWindow().getSize();
    HandleResize(winSize.x, winSize.y);

    std::cout << "Simulation started. Robots active: " << m_robots.size() << std::endl;
    std::cout << "Controls: Arrow keys = move robot, Scroll = zoom, Middle-click drag = pan" << std::endl;
    std::cout << "          E = toggle editor, 1-4 = select brush, Left-click = paint cell" << std::endl;
    std::cout << "          Right-click = send robot to target (A* pathfinding)" << std::endl;
}

void SimulationState::ProcessEvents() {
    sf::Event event;
    while (m_app.GetWindow().pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_app.GetWindow().close();
        }

        if (event.type == sf::Event::Resized) {
            HandleResize(event.size.width, event.size.height);
        }

        // Camera zoom with mouse wheel
        if (event.type == sf::Event::MouseWheelScrolled) {
            float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
            m_zoomLevel *= zoomFactor;
            m_view.zoom(zoomFactor);
        }

        // Camera pan with middle mouse button
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Middle) {
            m_isPanning = true;
            m_panStart = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        }
        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Middle) {
            m_isPanning = false;
        }
        if (event.type == sf::Event::MouseMoved && m_isPanning) {
            sf::Vector2i panEnd(event.mouseMove.x, event.mouseMove.y);
            sf::Vector2f delta = m_app.GetWindow().mapPixelToCoords(m_panStart, m_view)
                               - m_app.GetWindow().mapPixelToCoords(panEnd, m_view);
            m_view.move(delta);
            m_panStart = panEnd;
        }

        // Grid editor: left-click to paint cells
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left && m_editorActive) {
            sf::Vector2i screenPos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i cell = ScreenToGrid(screenPos);
            if (cell.x >= 0 && cell.x < m_grid.GetWidth() &&
                cell.y >= 0 && cell.y < m_grid.GetHeight()) {
                m_grid.SetCell(cell.x, cell.y, m_brushType);
            }
        }

        // Right-click: send first robot to target via A* pathfinding
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Right) {
            sf::Vector2i screenPos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i target = ScreenToGrid(screenPos);
            if (!m_robots.empty() &&
                target.x >= 0 && target.x < m_grid.GetWidth() &&
                target.y >= 0 && target.y < m_grid.GetHeight()) {
                m_robots[0].setAutonomous(true);
                m_robots[0].SetPathTarget(target, m_grid);
                std::cout << "Robot 1 pathfinding to (" << target.x << "," << target.y << ")" << std::endl;
            }
        }

        if (event.type == sf::Event::KeyPressed) {
            // Manual control of first robot
            if (!m_robots.empty()) {
                if (event.key.code == sf::Keyboard::Up ||
                    event.key.code == sf::Keyboard::Down ||
                    event.key.code == sf::Keyboard::Left ||
                    event.key.code == sf::Keyboard::Right) {
                    m_robots[0].setAutonomous(false);
                }
                if (event.key.code == sf::Keyboard::Up)
                    m_robots[0].TryMove(0, -1, m_grid, m_robots);
                if (event.key.code == sf::Keyboard::Down)
                    m_robots[0].TryMove(0, 1, m_grid, m_robots);
                if (event.key.code == sf::Keyboard::Left)
                    m_robots[0].TryMove(-1, 0, m_grid, m_robots);
                if (event.key.code == sf::Keyboard::Right)
                    m_robots[0].TryMove(1, 0, m_grid, m_robots);
            }

            // Toggle grid editor mode
            if (event.key.code == sf::Keyboard::E) {
                m_editorActive = !m_editorActive;
                std::cout << "Grid editor " << (m_editorActive ? "enabled" : "disabled") << std::endl;
            }

            // Brush selection: 1=Empty, 2=Wall, 3=Shelf, 4=ChargingStation
            if (event.key.code == sf::Keyboard::Num1) m_brushType = CellType::Empty;
            if (event.key.code == sf::Keyboard::Num2) m_brushType = CellType::Wall;
            if (event.key.code == sf::Keyboard::Num3) m_brushType = CellType::Shelf;
            if (event.key.code == sf::Keyboard::Num4) m_brushType = CellType::ChargingStation;

            if (event.key.code == sf::Keyboard::Escape) {
                m_app.ChangeState(std::make_unique<MainMenuState>(m_app));
                return;
            }
        }
    }
}

void SimulationState::Update(float dt) {
    for (auto& robot : m_robots) {
        robot.Update(dt, m_grid, m_robots);
    }
}

void SimulationState::Render(sf::RenderWindow& window) {
    window.setView(m_view);
    m_renderer.DrawGrid(window, m_grid);
    m_renderer.DrawRobots(window, m_robots);

    // Draw HUD overlay with battery info
    static constexpr float cellSize = 30.0f;
    static constexpr float hudMargin = 20.0f;
    static constexpr float hudTopOffset = -30.0f;
    float hudX = m_grid.GetWidth() * cellSize + hudMargin;
    float hudY = hudTopOffset;

    sf::Text hudTitle;
    hudTitle.setFont(m_font);
    hudTitle.setString("Robots");
    hudTitle.setCharacterSize(14);
    hudTitle.setFillColor(sf::Color::White);
    hudTitle.setPosition(hudX, hudY);
    window.draw(hudTitle);

    float yOff = hudY + 20.0f;
    for (const auto& robot : m_robots) {
        sf::Text info;
        info.setFont(m_font);

        std::string label = "R" + std::to_string(robot.getId()) + " Bat: "
            + std::to_string(static_cast<int>(robot.getBattery())) + "%";
        if (!robot.isAutonomous())
            label += " [Manual]";
        if (robot.hasPath())
            label += " [Path]";

        info.setString(label);
        info.setCharacterSize(10);

        sf::Color col = sf::Color::Green;
        if (robot.getBattery() < 30.0f) col = sf::Color::Red;
        else if (robot.getBattery() < 60.0f) col = sf::Color::Yellow;

        info.setFillColor(col);
        info.setPosition(hudX, yOff);
        window.draw(info);
        yOff += 16.0f;
    }

    // Editor mode indicator
    if (m_editorActive) {
        sf::Text editorText;
        editorText.setFont(m_font);
        std::string brushName;
        switch (m_brushType) {
            case CellType::Empty:           brushName = "Empty";    break;
            case CellType::Wall:            brushName = "Wall";     break;
            case CellType::Shelf:           brushName = "Shelf";    break;
            case CellType::ChargingStation: brushName = "Station";  break;
        }
        editorText.setString("EDITOR [" + brushName + "]");
        editorText.setCharacterSize(12);
        editorText.setFillColor(sf::Color::Magenta);
        editorText.setPosition(hudX, yOff + 10.0f);
        window.draw(editorText);
    }
}

void SimulationState::HandleResize(unsigned int width, unsigned int height) {
    m_view = m_app.GetLetterboxView(m_view, width, height);
    m_app.GetWindow().setView(m_view);
}

sf::Vector2i SimulationState::ScreenToGrid(sf::Vector2i screenPos) const {
    sf::Vector2f worldPos = m_app.GetWindow().mapPixelToCoords(screenPos, m_view);
    static constexpr float cellSize = 30.0f;
    int gx = static_cast<int>(worldPos.x / cellSize);
    int gy = static_cast<int>(worldPos.y / cellSize);
    return {gx, gy};
}
