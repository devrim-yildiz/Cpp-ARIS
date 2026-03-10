//
// Created by Devrim Yildiz on 26.01.26.
// Visual upgrade: sci-fi industrial look with SFML 2.6
//

#pragma once
#include <SFML/Graphics.hpp>
#include "../Simulation/Grid.h"
#include "Simulation/Robot.h"
#include <string>

class WorldRenderer {
public:
    /// Constructor: loads font and initialises visual resources.
    WorldRenderer();

    /// Draw the grid background, cell contents and grid lines.
    void DrawGrid(sf::RenderWindow& window, const Grid& grid, float elapsedTime);

    /// Draw all robots with composite visuals and battery bars.
    void DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots, float elapsedTime);

    /// Draw the HUD overlay (status panel, FPS, robot count).
    void DrawHUD(sf::RenderWindow& window, std::size_t robotCount, float elapsedTime);

    // ---- backward-compatible overloads (no elapsedTime) ----
    void DrawGrid(sf::RenderWindow& window, const Grid& grid);
    void DrawRobots(sf::RenderWindow& window, const std::vector<Robot>& robots);

private:
    // ---- Helper methods ----

    /// Draw a semi-transparent drop-shadow behind a shape.
    void DrawShadow(sf::RenderWindow& window, const sf::Shape& shape,
                    float offsetX = 3.f, float offsetY = 3.f,
                    sf::Uint8 alpha = 80) const;

    /// Draw a single cell (wall / shelf / charging station / empty).
    void DrawCell(sf::RenderWindow& window, int x, int y, CellType type,
                  float elapsedTime);

    /// Draw one robot (outer ring + inner core + direction indicator + battery).
    void DrawRobot(sf::RenderWindow& window, const Robot& robot,
                   float elapsedTime) const;

    /// Draw the grid-line overlay using a VertexArray.
    void DrawGridLines(sf::RenderWindow& window, int cols, int rows) const;

    // ---- Configuration ----
    float m_cellSize = 30.f;            ///< Pixel size per grid cell.

    // ---- Colour palette (sci-fi industrial) ----
    sf::Color m_bgColor{20, 25, 38};            ///< Dark navy background.
    sf::Color m_emptyColor{30, 36, 52};          ///< Empty cell fill.
    sf::Color m_wallColor{180, 190, 210};        ///< Light steel for walls.
    sf::Color m_shelfColor{100, 70, 40};         ///< Warm brown shelves.
    sf::Color m_gridLineColor{60, 70, 90, 60};   ///< Subtle semi-transparent lines.
    sf::Color m_chargingBase{20, 180, 80};        ///< Charging station base green.

    sf::Color m_robotRingColor{80, 90, 110};     ///< Outer ring (dark metallic).
    sf::Color m_robotCoreColor{0, 230, 255};     ///< Neon-cyan inner core.

    sf::Color m_hudBg{15, 18, 28, 200};          ///< Semi-transparent HUD panel.
    sf::Color m_hudText{180, 220, 255};           ///< Cool-white HUD text.

    // ---- Font & HUD ----
    sf::Font m_font;                    ///< FiraCode-Regular loaded from resources/FiraCode-Regular.ttf.
    bool     m_fontLoaded = false;      ///< Guard against missing font.

    // ---- FPS calculation ----
    sf::Clock m_fpsClock;               ///< Clock for FPS measurement.
    float     m_fps          = 0.f;     ///< Last computed FPS value.
    int       m_frameCount   = 0;       ///< Frames since last FPS update.
    float     m_fpsTimer     = 0.f;     ///< Accumulated time for FPS window.

    // ---- Easter-egg state ----
    float m_scanLineOffset = 0.f;       ///< Scrolling scan-line effect offset.
};