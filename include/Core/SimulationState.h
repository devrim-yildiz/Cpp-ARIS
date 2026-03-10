//
// SimulationState.h - Grid and robot simulation with camera controls and grid editor
//

#pragma once
#include "Core/State.h"
#include "Simulation/Grid.h"
#include "Simulation/Robot.h"
#include "Renderer/WorldRenderer.h"
#include <SFML/Graphics.hpp>
#include <vector>

class SimulationState : public State {
public:
    explicit SimulationState(Application& app);

    void ProcessEvents() override;
    void Update(float dt) override;
    void Render(sf::RenderWindow& window) override;

private:
    Grid m_grid;
    WorldRenderer m_renderer;
    std::vector<Robot> m_robots;
    sf::View m_view;
    sf::Font m_font;

    // Camera controls
    float m_zoomLevel = 1.0f;
    bool m_isPanning = false;
    sf::Vector2i m_panStart;

    // Grid editor
    CellType m_brushType = CellType::Wall;
    bool m_editorActive = false;

    void HandleResize(unsigned int width, unsigned int height);
    sf::Vector2i ScreenToGrid(sf::Vector2i screenPos) const;
};
