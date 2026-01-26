//
// Created by Devrim Yildiz on 26.01.26.
//

#include "UI/UIPanel.h"
#include <iostream>

UIPanel::UIPanel(const std::string& title, const sf::Vector2f& position, const sf::Vector2f& size)
    : m_title(title)
    , m_position(position)
    , m_size(size)
    , m_visible(true)
    , m_isDragging(false)
    , m_bgColor(sf::Color(30, 30, 35, 240))
    , m_titleBarColor(sf::Color(45, 45, 50, 255))
    , m_borderColor(sf::Color(70, 130, 180, 255))
    , m_textColor(sf::Color(220, 220, 225, 255))
{
    if (!m_font.loadFromFile("resources/FiraCode-Regular.ttf")) {
        std::cerr << "Warning: Could not load font for UI Panel" << std::endl;
    }
    
    UpdateShapes();
}

void UIPanel::SetPosition(const sf::Vector2f& position) {
    m_position = position;
    UpdateShapes();
}

void UIPanel::SetSize(const sf::Vector2f& size) {
    m_size = size;
    UpdateShapes();
}

void UIPanel::UpdateShapes() {
    // Background
    m_background.setPosition(m_position);
    m_background.setSize(m_size);
    m_background.setFillColor(m_bgColor);
    m_background.setOutlineColor(m_borderColor);
    m_background.setOutlineThickness(2.f);
    
    // Title bar
    m_titleBar.setPosition(m_position);
    m_titleBar.setSize(sf::Vector2f(m_size.x, 30.f));
    m_titleBar.setFillColor(m_titleBarColor);
}

bool UIPanel::IsPointInTitleBar(const sf::Vector2f& point) const {
    sf::FloatRect titleBarBounds(m_position.x, m_position.y, m_size.x, 30.f);
    return titleBarBounds.contains(point);
}

sf::FloatRect UIPanel::GetBounds() const {
    return sf::FloatRect(m_position, m_size);
}

void UIPanel::Update(float deltaTime) {
    // Base update - override in derived classes
}

void UIPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (!m_visible) return;
    
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (IsPointInTitleBar(mousePos)) {
                m_isDragging = true;
                m_dragOffset = mousePos - m_position;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            m_isDragging = false;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (m_isDragging) {
            SetPosition(mousePos - m_dragOffset);
        }
    }
}

void UIPanel::Render(sf::RenderWindow& window) {
    if (!m_visible) return;
    
    // Draw background
    window.draw(m_background);
    
    // Draw title bar
    window.draw(m_titleBar);
    
    // Draw title text
    sf::Text titleText;
    titleText.setFont(m_font);
    titleText.setString(m_title);
    titleText.setCharacterSize(16);
    titleText.setFillColor(m_textColor);
    titleText.setPosition(m_position.x + 10.f, m_position.y + 7.f);
    window.draw(titleText);
}
