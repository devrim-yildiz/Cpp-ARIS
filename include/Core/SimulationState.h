//
// SimulationState.h - Grid and robot simulation logic
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

    void HandleResize(unsigned int width, unsigned int height);
};
