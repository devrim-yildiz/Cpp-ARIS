//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include "../Simulation/Grid.h"

class WorldRenderer {
public:
    // Constructor might load textures or fonts
    WorldRenderer();

    // The Main Function
    // notice: const Grid& grid
    // We promise not to touch the data, just look at it.
    void Draw(sf::RenderWindow& window, const Grid& grid);

private:
    // Helper to draw a single cell, keeps the main Draw() clean
    void DrawCell(sf::RenderWindow& window, int x, int y, CellType type);

    // Configuration (hardcoded for now, config file later)
    float m_cellSize = 30.0f;
};