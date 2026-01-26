//
// Created by Devrim Yildiz on 26.01.26.
//

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

// Modern UI Panel System for A.R.I.S.
class UIPanel {
public:
    UIPanel(const std::string& title, const sf::Vector2f& position, const sf::Vector2f& size);
    virtual ~UIPanel() = default;
    
    virtual void Update(float deltaTime);
    virtual void Render(sf::RenderWindow& window);
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    
    void SetVisible(bool visible) { m_visible = visible; }
    [[nodiscard]] bool IsVisible() const { return m_visible; }
    
    void SetPosition(const sf::Vector2f& position);
    void SetSize(const sf::Vector2f& size);
    
    [[nodiscard]] const std::string& GetTitle() const { return m_title; }
    [[nodiscard]] sf::FloatRect GetBounds() const;

protected:
    std::string m_title;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_visible;
    bool m_isDragging;
    sf::Vector2f m_dragOffset;
    
    sf::RectangleShape m_background;
    sf::RectangleShape m_titleBar;
    sf::Font m_font;
    
    // Modern color scheme
    sf::Color m_bgColor;
    sf::Color m_titleBarColor;
    sf::Color m_borderColor;
    sf::Color m_textColor;
    
    void UpdateShapes();
    bool IsPointInTitleBar(const sf::Vector2f& point) const;
};
