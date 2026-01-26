//
// Created by Devrim Yildiz on 26.01.26.
//
#include "Core/Application.h"

Application::Application()
    : m_window(sf::VideoMode(800, 600), "ARIS Simulation"),
      m_grid(20, 15) // Initialize a 20x15 grid
{
    m_window.setFramerateLimit(60);

    // Setup some dummy data to verify it works
    m_grid.SetCell(5, 5, CellType::Wall);
    m_grid.SetCell(6, 5, CellType::Shelf);
}

void Application::Run() {
    while (m_window.isOpen()) {
        ProcessEvents();
        Update();
        Render();
    }
}

void Application::ProcessEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_window.close();
    }
}

void Application::Update() {
    // Logic updates (Robot movement) will go here later
}

void Application::Render() {
    m_window.clear();
    m_renderer.Draw(m_window, m_grid);
    m_window.display();
}