//
// MainMenuState.h - Main menu with title and buttons
//

#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class MainMenuState : public State {
public:
    explicit MainMenuState(Application& app);

    void ProcessEvents() override;
    void Update(float dt) override;
    void Render(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_startText;
    sf::Text m_exitText;
    sf::View m_view;

    void HandleResize(unsigned int width, unsigned int height);
    bool IsTextHovered(const sf::Text& text, const sf::Vector2f& mousePos) const;
};
