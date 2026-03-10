//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include "../Simulation/Grid.h"
#include "Simulation/Robot.h"

class WorldRenderer {
public:
    // Shared cell size constant
    static constexpr float CELL_SIZE = 30.0f;

    // Constructor might load textures or fonts
    WorldRenderer();

    // The Main Function
    // notice: const Grid& grid
    // We promise not to touch the data, just look at it.
    void DrawGrid(sf::RenderWindow& window, const Grid& grid);
    void DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots);

private:
    // Helper to draw a single cell, keeps the main Draw() clean
    void DrawCell(sf::RenderWindow& window, int x, int y, CellType type);

    // Configuration
    float m_cellSize = CELL_SIZE;
};