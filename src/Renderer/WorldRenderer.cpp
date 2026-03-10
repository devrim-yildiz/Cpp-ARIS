//
// Created by Devrim Yildiz on 26.01.26.
// Visual upgrade: sci-fi industrial look with SFML 2.6
//

#include "Renderer/WorldRenderer.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

WorldRenderer::WorldRenderer() {
    if (!m_font.loadFromFile("resources/FiraCode-Regular.ttf")) {
        std::cerr << "[WorldRenderer] Warning: Could not load FiraCode-Regular.ttf\n";
        m_fontLoaded = false;
    } else {
        m_fontLoaded = true;
    }
}

// ---------------------------------------------------------------------------
// Backward-compatible overloads (no elapsedTime)
// ---------------------------------------------------------------------------

void WorldRenderer::DrawGrid(sf::RenderWindow& window, const Grid& grid) {
    DrawGrid(window, grid, 0.f);
}

void WorldRenderer::DrawRobots(sf::RenderWindow& window,
                                const std::vector<Robot>& robots) {
    DrawRobots(window, robots, 0.f);
}

// ---------------------------------------------------------------------------
// Grid rendering
// ---------------------------------------------------------------------------

void WorldRenderer::DrawGrid(sf::RenderWindow& window, const Grid& grid,
                              float elapsedTime) {
    const int cols = grid.GetWidth();
    const int rows = grid.GetHeight();

    // 1. Solid dark background covering the entire grid area
    sf::RectangleShape bg(sf::Vector2f(cols * m_cellSize, rows * m_cellSize));
    bg.setFillColor(m_bgColor);
    window.draw(bg);

    // 2. Draw each cell
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            DrawCell(window, x, y, grid.GetCell(x, y), elapsedTime);
        }
    }

    // 3. Semi-transparent grid lines on top (VertexArray)
    DrawGridLines(window, cols, rows);
}

// ---------------------------------------------------------------------------
// Individual cell rendering with shadows
// ---------------------------------------------------------------------------

void WorldRenderer::DrawCell(sf::RenderWindow& window, int x, int y,
                              CellType type, float elapsedTime) {
    const float pad = 1.f;
    sf::RectangleShape cell(sf::Vector2f(m_cellSize - pad, m_cellSize - pad));
    cell.setPosition(x * m_cellSize + pad * 0.5f, y * m_cellSize + pad * 0.5f);

    switch (type) {
        case CellType::Empty:
            cell.setFillColor(m_emptyColor);
            window.draw(cell);
            break;

        case CellType::Wall:
            cell.setFillColor(m_wallColor);
            DrawShadow(window, cell);
            window.draw(cell);
            break;

        case CellType::Shelf:
            cell.setFillColor(m_shelfColor);
            DrawShadow(window, cell);
            window.draw(cell);
            // Tiny shelf detail (Easter egg #1: shelves show a little box icon)
            {
                sf::RectangleShape inner(sf::Vector2f(m_cellSize * 0.4f,
                                                       m_cellSize * 0.3f));
                inner.setFillColor(sf::Color(140, 100, 60, 180));
                inner.setPosition(x * m_cellSize + m_cellSize * 0.3f,
                                  y * m_cellSize + m_cellSize * 0.35f);
                window.draw(inner);
            }
            break;

        case CellType::ChargingStation: {
            // Pulsing neon glow effect using sin(elapsedTime)
            const float pulse = (std::sin(elapsedTime * 3.f) + 1.f) * 0.5f; // 0..1
            const sf::Uint8 alpha = static_cast<sf::Uint8>(120 + pulse * 135);
            sf::Color glow(m_chargingBase.r, m_chargingBase.g, m_chargingBase.b,
                           alpha);
            cell.setFillColor(glow);
            DrawShadow(window, cell, 3.f, 3.f, 50);
            window.draw(cell);

            // Lightning-bolt icon (Easter egg #2)
            sf::ConvexShape bolt;
            bolt.setPointCount(7);
            const float cx = x * m_cellSize + m_cellSize * 0.5f;
            const float cy = y * m_cellSize + m_cellSize * 0.5f;
            const float s  = m_cellSize * 0.18f;
            bolt.setPoint(0, sf::Vector2f(cx - s * 0.2f, cy - s * 1.6f));
            bolt.setPoint(1, sf::Vector2f(cx + s * 0.6f, cy - s * 0.2f));
            bolt.setPoint(2, sf::Vector2f(cx + s * 0.05f, cy - s * 0.2f));
            bolt.setPoint(3, sf::Vector2f(cx + s * 0.2f, cy + s * 1.6f));
            bolt.setPoint(4, sf::Vector2f(cx - s * 0.6f, cy + s * 0.2f));
            bolt.setPoint(5, sf::Vector2f(cx - s * 0.05f, cy + s * 0.2f));
            bolt.setPoint(6, sf::Vector2f(cx - s * 0.2f, cy - s * 1.6f));
            bolt.setFillColor(sf::Color(255, 255, 100,
                              static_cast<sf::Uint8>(150 + pulse * 105)));
            window.draw(bolt);
            break;
        }

        default:
            cell.setFillColor(sf::Color::Magenta); // Unknown type – keep visible
            window.draw(cell);
            break;
    }
}

// ---------------------------------------------------------------------------
// Grid lines (sf::VertexArray – Lines)
// ---------------------------------------------------------------------------

void WorldRenderer::DrawGridLines(sf::RenderWindow& window, int cols,
                                   int rows) const {
    const float w = cols * m_cellSize;
    const float h = rows * m_cellSize;
    const std::size_t lineCount =
        static_cast<std::size_t>(cols + 1 + rows + 1);
    sf::VertexArray lines(sf::Lines, lineCount * 2);

    std::size_t idx = 0;

    // Vertical lines
    for (int x = 0; x <= cols; ++x) {
        const float px = x * m_cellSize;
        lines[idx].position = sf::Vector2f(px, 0.f);
        lines[idx].color    = m_gridLineColor;
        ++idx;
        lines[idx].position = sf::Vector2f(px, h);
        lines[idx].color    = m_gridLineColor;
        ++idx;
    }

    // Horizontal lines
    for (int y = 0; y <= rows; ++y) {
        const float py = y * m_cellSize;
        lines[idx].position = sf::Vector2f(0.f, py);
        lines[idx].color    = m_gridLineColor;
        ++idx;
        lines[idx].position = sf::Vector2f(w, py);
        lines[idx].color    = m_gridLineColor;
        ++idx;
    }

    window.draw(lines);
}

// ---------------------------------------------------------------------------
// Drop-shadow helper
// ---------------------------------------------------------------------------

void WorldRenderer::DrawShadow(sf::RenderWindow& window,
                                const sf::Shape& shape, float offsetX,
                                float offsetY, sf::Uint8 alpha) const {
    // Clone position + size via global bounds (works for RectangleShapes).
    const auto bounds = shape.getGlobalBounds();
    sf::RectangleShape shadow(sf::Vector2f(bounds.width, bounds.height));
    shadow.setPosition(bounds.left + offsetX, bounds.top + offsetY);
    shadow.setFillColor(sf::Color(0, 0, 0, alpha));
    window.draw(shadow);
}

// ---------------------------------------------------------------------------
// Robot rendering
// ---------------------------------------------------------------------------

void WorldRenderer::DrawRobots(sf::RenderWindow& window,
                                const std::vector<Robot>& robots,
                                float elapsedTime) {
    for (const auto& robot : robots) {
        DrawRobot(window, robot, elapsedTime);
    }
}

void WorldRenderer::DrawRobot(sf::RenderWindow& window, const Robot& robot,
                               float elapsedTime) const {
    const float cx = robot.getX() * m_cellSize + m_cellSize * 0.5f;
    const float cy = robot.getY() * m_cellSize + m_cellSize * 0.5f;

    const float outerR = m_cellSize * 0.40f;
    const float innerR = m_cellSize * 0.26f;

    // --- Shadow ---
    sf::CircleShape shadowCircle(outerR);
    shadowCircle.setOrigin(outerR, outerR);
    shadowCircle.setPosition(cx + 3.f, cy + 3.f);
    shadowCircle.setFillColor(sf::Color(0, 0, 0, 70));
    window.draw(shadowCircle);

    // --- Outer ring (dark metallic) ---
    sf::CircleShape outerRing(outerR);
    outerRing.setOrigin(outerR, outerR);
    outerRing.setPosition(cx, cy);
    outerRing.setFillColor(m_robotRingColor);
    outerRing.setOutlineThickness(1.5f);
    outerRing.setOutlineColor(sf::Color(120, 135, 160));
    window.draw(outerRing);

    // --- Inner neon-cyan core (subtle pulse) ---
    const float pulse = (std::sin(elapsedTime * 4.f + robot.getId() * 1.5f)
                         + 1.f) * 0.5f;
    sf::Uint8 coreAlpha = static_cast<sf::Uint8>(180 + pulse * 75);
    sf::CircleShape core(innerR);
    core.setOrigin(innerR, innerR);
    core.setPosition(cx, cy);
    core.setFillColor(sf::Color(m_robotCoreColor.r, m_robotCoreColor.g,
                                 m_robotCoreColor.b, coreAlpha));
    window.draw(core);

    // --- Direction indicator (small white triangle pointing up by default) ---
    {
        const float arrH = m_cellSize * 0.16f;
        const float arrW = m_cellSize * 0.10f;
        sf::ConvexShape arrow;
        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f(cx, cy - innerR + 1.f));
        arrow.setPoint(1, sf::Vector2f(cx - arrW, cy - innerR + arrH + 1.f));
        arrow.setPoint(2, sf::Vector2f(cx + arrW, cy - innerR + arrH + 1.f));
        arrow.setFillColor(sf::Color(255, 255, 255, 210));
        window.draw(arrow);
    }

    // --- Battery bar (above robot) ---
    {
        const float barW = m_cellSize * 0.6f;
        const float barH = 4.f;
        const float barX = cx - barW * 0.5f;
        const float barY = cy - outerR - barH - 3.f;
        const float batteryPct = 1.f; // 100% – placeholder until battery system

        sf::RectangleShape barBg(sf::Vector2f(barW, barH));
        barBg.setPosition(barX, barY);
        barBg.setFillColor(sf::Color(40, 40, 40, 200));
        window.draw(barBg);

        sf::RectangleShape barFill(
            sf::Vector2f(barW * std::clamp(batteryPct, 0.f, 1.f), barH));
        barFill.setPosition(barX, barY);
        // Green when full, amber when mid, red when low
        sf::Color fillCol = batteryPct > 0.5f
                                ? sf::Color(0, 220, 80, 230)
                                : (batteryPct > 0.2f
                                       ? sf::Color(240, 180, 0, 230)
                                       : sf::Color(240, 40, 40, 230));
        barFill.setFillColor(fillCol);
        window.draw(barFill);
    }

    // --- Easter egg #3: robot ID label ---
    if (m_fontLoaded) {
        sf::Text idLabel;
        idLabel.setFont(m_font);
        idLabel.setCharacterSize(10);
        idLabel.setFillColor(sf::Color(255, 255, 255, 180));
        idLabel.setString(std::to_string(robot.getId()));
        const auto tb = idLabel.getLocalBounds();
        idLabel.setOrigin(tb.left + tb.width * 0.5f,
                          tb.top + tb.height * 0.5f);
        idLabel.setPosition(cx, cy + 1.f);
        window.draw(idLabel);
    }
}

// ---------------------------------------------------------------------------
// HUD overlay
// ---------------------------------------------------------------------------

void WorldRenderer::DrawHUD(sf::RenderWindow& window, std::size_t robotCount,
                             float elapsedTime) {
    if (!m_fontLoaded) return;

    const auto winSize = window.getSize();
    const float panelH = 36.f;

    // --- FPS calculation (update every 0.5 s) ---
    const float dt = m_fpsClock.restart().asSeconds();
    m_fpsTimer += dt;
    ++m_frameCount;
    if (m_fpsTimer >= 0.5f) {
        m_fps = static_cast<float>(m_frameCount) / m_fpsTimer;
        m_frameCount = 0;
        m_fpsTimer   = 0.f;
    }

    // --- Semi-transparent panel at the top ---
    sf::RectangleShape panel(sf::Vector2f(static_cast<float>(winSize.x),
                                           panelH));
    panel.setFillColor(m_hudBg);
    window.draw(panel);

    // Thin accent line under panel (Easter egg #4: neon accent)
    {
        sf::RectangleShape accent(sf::Vector2f(static_cast<float>(winSize.x),
                                                2.f));
        accent.setPosition(0.f, panelH);
        const float t = (std::sin(elapsedTime * 2.f) + 1.f) * 0.5f;
        accent.setFillColor(sf::Color(
            static_cast<sf::Uint8>(0 + t * 60),
            static_cast<sf::Uint8>(180 + t * 50),
            static_cast<sf::Uint8>(255),
            200));
        window.draw(accent);
    }

    // --- Text elements ---
    const float textY   = 8.f;
    const unsigned csize = 14;

    // Title
    sf::Text title;
    title.setFont(m_font);
    title.setCharacterSize(csize);
    title.setFillColor(m_hudText);
    title.setStyle(sf::Text::Bold);
    title.setString("A.R.I.S. SIMULATION ACTIVE");
    title.setPosition(12.f, textY);
    window.draw(title);

    // Robot count
    sf::Text robotsText;
    robotsText.setFont(m_font);
    robotsText.setCharacterSize(csize);
    robotsText.setFillColor(m_hudText);
    robotsText.setString("Robots: " + std::to_string(robotCount));
    robotsText.setPosition(320.f, textY);
    window.draw(robotsText);

    // FPS
    {
        std::ostringstream oss;
        oss.precision(0);
        oss << std::fixed << "FPS: " << m_fps;
        sf::Text fpsText;
        fpsText.setFont(m_font);
        fpsText.setCharacterSize(csize);
        fpsText.setFillColor(sf::Color(100, 255, 130));
        fpsText.setString(oss.str());
        fpsText.setPosition(static_cast<float>(winSize.x) - 120.f, textY);
        window.draw(fpsText);
    }

    // --- Easter egg #5: scrolling scan-line (CRT effect) ---
    {
        m_scanLineOffset += dt * 80.f;
        if (m_scanLineOffset > static_cast<float>(winSize.y))
            m_scanLineOffset = 0.f;
        sf::RectangleShape scanLine(
            sf::Vector2f(static_cast<float>(winSize.x), 1.f));
        scanLine.setPosition(0.f, m_scanLineOffset);
        scanLine.setFillColor(sf::Color(180, 255, 220, 12));
        window.draw(scanLine);
    }
}
