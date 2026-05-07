#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#include "World.h"

int main()
{
    const unsigned int width = 800;
    const unsigned int height = 600;
	const unsigned int num_particles = 1000;


	World world(num_particles, width, height);


    sf::RenderWindow window(sf::VideoMode({ width, height }), "SFML 3 Test Render");

    sf::Image image({ width, height }, sf::Color::Black);

    // Render once, like a ray tracer frame
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            float texture_u = static_cast<float>(x) / static_cast<float>(width);
            float texture_v = static_cast<float>(y) / static_cast<float>(height);

            std::uint8_t red = static_cast<std::uint8_t>(255 * texture_u);
            std::uint8_t green = static_cast<std::uint8_t>(255 * texture_v);
            std::uint8_t blue = 100;

            float center_x = width / 2.0f;
            float center_y = height / 2.0f;
            float delta_x = static_cast<float>(x) - center_x;
            float delta_y = static_cast<float>(y) - center_y;
            float distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);

            if (distance < 100.0f)
            {
                red = 255;
                green = 50;
                blue = 50;
            }

            image.setPixel({ x, y }, sf::Color(red, green, blue));
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