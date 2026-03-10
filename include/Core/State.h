//
// State.h - Base State interface for the Game State Machine
//

#pragma once
#include <SFML/Graphics.hpp>

class Application;

class State {
public:
    explicit State(Application& app) : m_app(app) {}
    virtual ~State() = default;

    virtual void ProcessEvents() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(sf::RenderWindow& window) = 0;

protected:
    Application& m_app;
};
