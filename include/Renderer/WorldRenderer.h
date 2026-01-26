//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include "../Simulation/Grid.h"
#include "Simulation/Robot.h"
#include "Core/Utils.h"

class WorldRenderer {
public:
    WorldRenderer();

    void DrawGrid(sf::RenderWindow& window, const Grid& grid, bool heatmapMode = false);
    void DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots);
    void DrawPath(sf::RenderWindow& window, const std::vector<Utils::Point>& path, const sf::Color& color);

private:
    void DrawCell(sf::RenderWindow& window, int x, int y, CellType type);

    float m_cellSize = 30.0f;
};