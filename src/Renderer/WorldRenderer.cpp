//
// Created by Devrim Yildiz on 26.01.26.
//

#include "Renderer/WorldRenderer.h"
WorldRenderer::WorldRenderer() {
    // Empty for now. Later we will load textures here.
}
void WorldRenderer::DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots) {
    float robotSize = m_cellSize * 0.8f; // Make them slightly smaller than the cell
    float offset = (m_cellSize - robotSize) / 2.0f; // Center them

    sf::CircleShape robotShape(robotSize / 2.0f); // Radius is half the size
    robotShape.setFillColor(sf::Color::Cyan);

    for (const auto& robot : robots) {
        // Use smooth lerped render position instead of snapping to grid
        float pixelX = robot.getRenderX() * m_cellSize + offset;
        float pixelY = robot.getRenderY() * m_cellSize + offset;

        robotShape.setPosition(pixelX, pixelY);
        window.draw(robotShape);
    }
}
void WorldRenderer::DrawGrid(sf::RenderWindow& window, const Grid& grid) {
    sf::RectangleShape cellShape(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f));

    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            cellShape.setPosition(x * m_cellSize, y * m_cellSize);

            CellType type = grid.GetCell(x, y);
            switch (type) {
            case CellType::Empty:           cellShape.setFillColor(sf::Color(50, 50, 50));  break;
            case CellType::Wall:            cellShape.setFillColor(sf::Color::White);       break;
            case CellType::Shelf:           cellShape.setFillColor(sf::Color(139, 69, 19)); break;
            case CellType::ChargingStation: cellShape.setFillColor(sf::Color(0, 200, 0));   break;
            }
            window.draw(cellShape);
        }
    }
}
