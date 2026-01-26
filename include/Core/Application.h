//
// Created by Devrim Yildiz on 26.01.26.
//

#ifndef A_R_I_S_APPLICATION_H
#define A_R_I_S_APPLICATION_H
#pragma once
#include <SFML/Graphics.hpp>
#include "Simulation/Grid.h"
#include "Renderer/WorldRenderer.h"

class Application {
public:
    Application(); // Constructor: Sets up window, initializes Grid
    void Run();    // The "Game Loop": HandleInput -> Update -> Render

private:
    void ProcessEvents(); // Keyboard/Mouse inputs
    void Update();        // Move robots, update logic
    void Render();        // clear -> worldRenderer.Draw() -> display

private:
    // The Core Systems
    sf::RenderWindow m_window;
    Grid m_grid;
    WorldRenderer m_renderer;

    // Maybe some clock for timing delta-time
    sf::Clock m_clock;
};
#endif //A_R_I_S_APPLICATION_H