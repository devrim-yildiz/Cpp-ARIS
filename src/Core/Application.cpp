//
// Application.cpp - Core application implementation
//
#include "Core/Application.h"
#include "Core/MainMenuState.h"
#include <iostream>

Application::Application()
    : m_window(sf::VideoMode::getDesktopMode(), "A.R.I.S.", sf::Style::Default)
{
    m_window.setFramerateLimit(60);

    // Start with the main menu
    PushState(std::make_unique<MainMenuState>(*this));

    std::cout << "A.R.I.S. Engine Initialized." << std::endl;
}

void Application::Run() {
    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();

        if (!m_states.empty()) {
            State* current = m_states.top().get();
            current->ProcessEvents();

            // State may have changed during ProcessEvents; verify before continuing
            if (!m_states.empty() && m_states.top().get() == current) {
                current->Update(dt);
                m_window.clear(sf::Color::Black);
                current->Render(m_window);
                m_window.display();
            }
        } else {
            m_window.close();
        }
    }
}

void Application::PushState(std::unique_ptr<State> state) {
    m_states.push(std::move(state));
}

void Application::PopState() {
    if (!m_states.empty()) {
        m_states.pop();
    }
}

void Application::ChangeState(std::unique_ptr<State> state) {
    if (!m_states.empty()) {
        m_states.pop();
    }
    m_states.push(std::move(state));
}

sf::RenderWindow& Application::GetWindow() {
    return m_window;
}

sf::View Application::GetLetterboxView(const sf::View& view, unsigned int windowWidth, unsigned int windowHeight) const {
    sf::View result = view;

    float windowRatio  = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float viewRatio    = view.getSize().x / view.getSize().y;

    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX  = 0.0f;
    float posY  = 0.0f;

    if (windowRatio >= viewRatio) {
        // Window wider than view -> pillarboxing
        sizeX = viewRatio / windowRatio;
        posX  = (1.0f - sizeX) / 2.0f;
    } else {
        // Window taller than view -> letterboxing
        sizeY = windowRatio / viewRatio;
        posY  = (1.0f - sizeY) / 2.0f;
    }

    result.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    return result;
}