#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "Version.hpp"
#include "Simulation/Robot.h"

static void modifyCurrentWorkingDirectory();
static void setupVersionTexts(sf::RenderWindow &window, sf::Font &font, sf::Text &templateVersion, sf::Text &sfmlVersion);

int main()
{
    modifyCurrentWorkingDirectory();

    const auto clearColor = sf::Color(234, 240, 206);
    auto title = "A.R.I.S Simulation";
    sf::RenderWindow window(sf::VideoMode(1280, 720), title, sf::Style::Close);

    sf::Font font;
    sf::Text sfmlVersion;
    sf::Text templateVersion;

    Robot myRobot(1, 0, 0);
    std::cout << "Roboter ID: " << myRobot.getId() << " gestartet." << std::endl;
    std::cout << "Position: " << myRobot.getX() << ", " << myRobot.getY() << std::endl;

    std::cout << "Bewege Roboter nach rechts..." << std::endl;
    myRobot.move(1, 0); // x+1, y+0

    std::cout << "Neue Position: " << myRobot.getX() << ", " << myRobot.getY() << std::endl;

    if (font.loadFromFile("resources/FiraCode-Regular.ttf"))
    {
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(clearColor);
        window.draw(templateVersion);
        window.draw(sfmlVersion);
        window.display();
    }

    return 0;
}

void modifyCurrentWorkingDirectory()
{
    while (!std::filesystem::exists("resources"))
    {
        std::filesystem::current_path(std::filesystem::current_path().parent_path());
    }
    auto cwd = std::filesystem::current_path();
}
