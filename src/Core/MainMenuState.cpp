//
// MainMenuState.cpp - Main menu with title and Start/Exit buttons
//
#include "Core/MainMenuState.h"
#include "Core/Application.h"
#include "Core/SimulationState.h"
#include <iostream>

MainMenuState::MainMenuState(Application& app)
    : State(app)
{
    if (!m_font.loadFromFile("resources/FiraCode-Regular.ttf")) {
        std::cerr << "Failed to load FiraCode-Regular.ttf" << std::endl;
    }

    m_view.setSize(static_cast<float>(Application::DESIGN_WIDTH),
                   static_cast<float>(Application::DESIGN_HEIGHT));
    m_view.setCenter(Application::DESIGN_WIDTH / 2.0f,
                     Application::DESIGN_HEIGHT / 2.0f);

    // Title
    m_titleText.setFont(m_font);
    m_titleText.setString("A.R.I.S.");
    m_titleText.setCharacterSize(120);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                          titleBounds.top + titleBounds.height / 2.0f);
    m_titleText.setPosition(Application::DESIGN_WIDTH / 2.0f, 350.0f);

    // Start Simulation button
    m_startText.setFont(m_font);
    m_startText.setString("Start Simulation");
    m_startText.setCharacterSize(48);
    m_startText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect startBounds = m_startText.getLocalBounds();
    m_startText.setOrigin(startBounds.left + startBounds.width / 2.0f,
                          startBounds.top + startBounds.height / 2.0f);
    m_startText.setPosition(Application::DESIGN_WIDTH / 2.0f, 580.0f);

    // Exit button
    m_exitText.setFont(m_font);
    m_exitText.setString("Exit");
    m_exitText.setCharacterSize(48);
    m_exitText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect exitBounds = m_exitText.getLocalBounds();
    m_exitText.setOrigin(exitBounds.left + exitBounds.width / 2.0f,
                         exitBounds.top + exitBounds.height / 2.0f);
    m_exitText.setPosition(Application::DESIGN_WIDTH / 2.0f, 680.0f);

    // Apply initial letterbox view
    sf::Vector2u winSize = m_app.GetWindow().getSize();
    HandleResize(winSize.x, winSize.y);
}

void MainMenuState::ProcessEvents() {
    sf::Event event;
    while (m_app.GetWindow().pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_app.GetWindow().close();
        }

        if (event.type == sf::Event::Resized) {
            HandleResize(event.size.width, event.size.height);
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos = m_app.GetWindow().mapPixelToCoords(
                sf::Mouse::getPosition(m_app.GetWindow()), m_view);

            if (IsTextHovered(m_startText, mousePos)) {
                m_app.ChangeState(std::make_unique<SimulationState>(m_app));
                return;
            }
            if (IsTextHovered(m_exitText, mousePos)) {
                m_app.GetWindow().close();
                return;
            }
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                m_app.ChangeState(std::make_unique<SimulationState>(m_app));
                return;
            }
            if (event.key.code == sf::Keyboard::Escape) {
                m_app.GetWindow().close();
                return;
            }
        }
    }
}

void MainMenuState::Update(float dt) {
    // Hover highlight
    sf::Vector2f mousePos = m_app.GetWindow().mapPixelToCoords(
        sf::Mouse::getPosition(m_app.GetWindow()), m_view);

    m_startText.setFillColor(IsTextHovered(m_startText, mousePos)
        ? sf::Color::Cyan : sf::Color(200, 200, 200));
    m_exitText.setFillColor(IsTextHovered(m_exitText, mousePos)
        ? sf::Color::Cyan : sf::Color(200, 200, 200));
}

void MainMenuState::Render(sf::RenderWindow& window) {
    window.setView(m_view);
    window.draw(m_titleText);
    window.draw(m_startText);
    window.draw(m_exitText);
}

void MainMenuState::HandleResize(unsigned int width, unsigned int height) {
    m_view = m_app.GetLetterboxView(m_view, width, height);
    m_app.GetWindow().setView(m_view);
}

bool MainMenuState::IsTextHovered(const sf::Text& text, const sf::Vector2f& mousePos) const {
    return text.getGlobalBounds().contains(mousePos);
}
