//
// Created by Devrim Yildiz on 26.01.26.
//

#include "Renderer/WorldRenderer.h"
WorldRenderer::WorldRenderer() {
    // Empty for now. Later we will load textures here.
}
void WorldRenderer::Draw(sf::RenderWindow& window, const Grid& grid) {
    sf::RectangleShape cellShape(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f));

    for (int y = 0; y < grid.GetHeight(); ++y) {
        for (int x = 0; x < grid.GetWidth(); ++x) {
            cellShape.setPosition(x * m_cellSize, y * m_cellSize);

            CellType type = grid.GetCell(x, y);
            switch (type) {
            case CellType::Empty:  cellShape.setFillColor(sf::Color(50, 50, 50)); break; // Dark Grey
            case CellType::Wall:   cellShape.setFillColor(sf::Color::White);      break;
            case CellType::Shelf:  cellShape.setFillColor(sf::Color(139, 69, 19)); break; // Brown
            default:               cellShape.setFillColor(sf::Color::Magenta);    break;
            }
            window.draw(cellShape);
        }
    }
}