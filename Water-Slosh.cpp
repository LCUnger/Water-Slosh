#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#include "FieldTypes.h"

int main()
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    sf::RenderWindow window(sf::VideoMode({ width, height }), "SFML 3 Test Render");

    sf::Image image({ width, height }, sf::Color::Black);

    // Render once, like a ray tracer frame
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float u = static_cast<float>(x) / static_cast<float>(width);
            float v = static_cast<float>(y) / static_cast<float>(height);

            std::uint8_t r = static_cast<std::uint8_t>(255 * u);
            std::uint8_t g = static_cast<std::uint8_t>(255 * v);
            std::uint8_t b = 100;

            float cx = width / 2.0f;
            float cy = height / 2.0f;
            float dx = static_cast<float>(x) - cx;
            float dy = static_cast<float>(y) - cy;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < 100.0f)
            {
                r = 255;
                g = 50;
                b = 50;
            }

            image.setPixel({ x, y }, sf::Color(r, g, b));
        }
    }

    sf::Texture texture(image);
    sf::Sprite sprite(texture);

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}