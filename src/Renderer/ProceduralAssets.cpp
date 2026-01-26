#include "Renderer/ProceduralAssets.h"
#include <cmath>

namespace ProceduralAssets {

sf::Texture GenerateGridTexture(int width, int height, const sf::Color& color) {
    sf::Image image;
    image.create(width, height, color);
    
    sf::Texture texture;
    texture.loadFromImage(image);
    return texture;
}

sf::Texture GenerateCheckerboardTexture(int size, const sf::Color& color1, const sf::Color& color2) {
    sf::Image image;
    image.create(size, size);
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            bool isColor1 = ((x / (size / 2)) + (y / (size / 2))) % 2 == 0;
            image.setPixel(x, y, isColor1 ? color1 : color2);
        }
    }
    
    sf::Texture texture;
    texture.loadFromImage(image);
    return texture;
}

sf::Texture GenerateRobotTexture(int size, const sf::Color& color) {
    sf::Image image;
    image.create(size, size, sf::Color::Transparent);
    
    int centerX = size / 2;
    int centerY = size / 2;
    int radius = size / 2 - 2;
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius) {
                image.setPixel(x, y, color);
            }
        }
    }
    
    sf::Texture texture;
    texture.loadFromImage(image);
    return texture;
}

sf::Texture GenerateItemTexture(int size) {
    sf::Image image;
    image.create(size, size, sf::Color::Transparent);
    
    int centerX = size / 2;
    int centerY = size / 2;
    int halfSize = size / 4;
    
    for (int y = centerY - halfSize; y < centerY + halfSize; ++y) {
        for (int x = centerX - halfSize; x < centerX + halfSize; ++x) {
            if (x >= 0 && x < size && y >= 0 && y < size) {
                image.setPixel(x, y, sf::Color::Yellow);
            }
        }
    }
    
    sf::Texture texture;
    texture.loadFromImage(image);
    return texture;
}

sf::Color InterpolateColor(const sf::Color& color1, const sf::Color& color2, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + (color2.r - color1.r) * t),
        static_cast<sf::Uint8>(color1.g + (color2.g - color1.g) * t),
        static_cast<sf::Uint8>(color1.b + (color2.b - color1.b) * t),
        static_cast<sf::Uint8>(color1.a + (color2.a - color1.a) * t)
    );
}

sf::Color GetHeatmapColor(float intensity) {
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 1.0f) intensity = 1.0f;
    
    sf::Color blue(0, 0, 255);
    sf::Color cyan(0, 255, 255);
    sf::Color green(0, 255, 0);
    sf::Color yellow(255, 255, 0);
    sf::Color red(255, 0, 0);
    
    if (intensity < 0.25f) {
        return InterpolateColor(blue, cyan, intensity * 4.0f);
    } else if (intensity < 0.5f) {
        return InterpolateColor(cyan, green, (intensity - 0.25f) * 4.0f);
    } else if (intensity < 0.75f) {
        return InterpolateColor(green, yellow, (intensity - 0.5f) * 4.0f);
    } else {
        return InterpolateColor(yellow, red, (intensity - 0.75f) * 4.0f);
    }
}

}
