#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

namespace ProceduralAssets {

sf::Texture GenerateGridTexture(int width, int height, const sf::Color& color);
sf::Texture GenerateCheckerboardTexture(int size, const sf::Color& color1, const sf::Color& color2);
sf::Texture GenerateRobotTexture(int size, const sf::Color& color);
sf::Texture GenerateItemTexture(int size);

sf::Color InterpolateColor(const sf::Color& color1, const sf::Color& color2, float t);
sf::Color GetHeatmapColor(float intensity);

}
