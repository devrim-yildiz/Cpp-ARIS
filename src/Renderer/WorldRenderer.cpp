//
// Created by Devrim Yildiz on 26.01.26.
//

#include "Renderer/WorldRenderer.h"
#include "Renderer/ProceduralAssets.h"

WorldRenderer::WorldRenderer() {
}

void WorldRenderer::DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots) {
    float robotSize = m_cellSize * 0.8f;
    float offset = (m_cellSize - robotSize) / 2.0f;

    sf::CircleShape robotShape(robotSize / 2.0f);

    for (const auto& robot : robots) {
        float pixelX = robot.getX() * m_cellSize + offset;
        float pixelY = robot.getY() * m_cellSize + offset;

        if (robot.GetState() == RobotState::LOW_BATTERY || robot.GetState() == RobotState::CHARGING) {
            robotShape.setFillColor(sf::Color::Red);
        } else if (robot.IsCarryingItem()) {
            robotShape.setFillColor(sf::Color::Green);
        } else {
            robotShape.setFillColor(sf::Color::Cyan);
        }

        robotShape.setPosition(pixelX, pixelY);
        window.draw(robotShape);
        
        float batteryBarWidth = robotSize;
        float batteryBarHeight = 3.0f;
        sf::RectangleShape batteryBar(sf::Vector2f(batteryBarWidth * (robot.GetBattery() / 100.0f), batteryBarHeight));
        batteryBar.setFillColor(robot.GetBattery() > 20.0f ? sf::Color::Green : sf::Color::Red);
        batteryBar.setPosition(pixelX, pixelY - 5.0f);
        window.draw(batteryBar);
    }
}

void WorldRenderer::DrawGrid(sf::RenderWindow& window, const Grid& grid, bool heatmapMode) {
    sf::RectangleShape cellShape(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f));
    
    int maxTraffic = 1;
    if (heatmapMode) {
        for (int y = 0; y < grid.GetHeight(); ++y) {
            for (int x = 0; x < grid.GetWidth(); ++x) {
                int traffic = grid.GetTraffic(x, y);
                if (traffic > maxTraffic) maxTraffic = traffic;
            }
        }
    }

    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            cellShape.setPosition(x * m_cellSize, y * m_cellSize);

            CellType type = grid.GetCell(x, y);
            
            if (heatmapMode && type == CellType::Empty) {
                float intensity = static_cast<float>(grid.GetTraffic(x, y)) / static_cast<float>(maxTraffic);
                cellShape.setFillColor(ProceduralAssets::GetHeatmapColor(intensity * 0.5f));
            } else {
                switch (type) {
                case CellType::Empty:           cellShape.setFillColor(sf::Color(50, 50, 50)); break;
                case CellType::Wall:            cellShape.setFillColor(sf::Color::White); break;
                case CellType::Shelf:           cellShape.setFillColor(sf::Color(139, 69, 19)); break;
                case CellType::ChargingStation: cellShape.setFillColor(sf::Color(255, 215, 0)); break;
                case CellType::PickupZone:      cellShape.setFillColor(sf::Color(0, 255, 0)); break;
                case CellType::DropZone:        cellShape.setFillColor(sf::Color(0, 0, 255)); break;
                default:                        cellShape.setFillColor(sf::Color::Magenta); break;
                }
            }
            window.draw(cellShape);
        }
    }
}

void WorldRenderer::DrawPath(sf::RenderWindow& window, const std::vector<Utils::Point>& path, const sf::Color& color) {
    if (path.size() < 2) return;
    
    for (size_t i = 0; i < path.size() - 1; ++i) {
        float x1 = path[i].x * m_cellSize + m_cellSize / 2.0f;
        float y1 = path[i].y * m_cellSize + m_cellSize / 2.0f;
        float x2 = path[i + 1].x * m_cellSize + m_cellSize / 2.0f;
        float y2 = path[i + 1].y * m_cellSize + m_cellSize / 2.0f;
        
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x1, y1), color),
            sf::Vertex(sf::Vector2f(x2, y2), color)
        };
        
        window.draw(line, 2, sf::Lines);
    }
}

