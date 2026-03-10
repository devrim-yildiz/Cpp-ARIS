//
// Application.h - Core application managing window, clock, and state stack
//

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "Core/State.h"

class Application {
public:
    Application();
    void Run();

    void PushState(std::unique_ptr<State> state);
    void PopState();
    void ChangeState(std::unique_ptr<State> state);

    sf::RenderWindow& GetWindow();
    sf::View GetLetterboxView(const sf::View& view, unsigned int windowWidth, unsigned int windowHeight) const;

    static constexpr unsigned int DESIGN_WIDTH  = 1920;
    static constexpr unsigned int DESIGN_HEIGHT = 1080;

private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    std::stack<std::unique_ptr<State>> m_states;
};