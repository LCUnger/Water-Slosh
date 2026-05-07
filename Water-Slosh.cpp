#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#include "World.h"

struct WindowConfig
{
    unsigned int width_px = 800;
    unsigned int height_px = 600;
};

int main()
{
    const WindowConfig window_config;
    const SimulationConfig simulation_config{
        .num_particles = 1000,
        .grid_width = 100,
        .grid_height = 60,
        .cell_size_m = 0.05
    };
    const RenderConfig render_config{
        .particle_radius_px = 2.0f,
        .pixels_per_meter = 100.0f
    };

	World world(simulation_config);
    ParticleRenderer particle_renderer(render_config);

	World world(num_particles, width, height);


    sf::RenderWindow window(sf::VideoMode({ width, height }), "SFML 3 Test Render");

    sf::Image image({ width, height }, sf::Color::Black);

    // Render once, like a ray tracer frame
    for (unsigned int y = 0; y < window_config.height_px; ++y)
    {
        for (unsigned int x = 0; x < window_config.width_px; ++x)
        {
            float texture_u = static_cast<float>(x) / static_cast<float>(width);
            float texture_v = static_cast<float>(y) / static_cast<float>(height);

            std::uint8_t red = static_cast<std::uint8_t>(255 * texture_u);
            std::uint8_t green = static_cast<std::uint8_t>(255 * texture_v);
            std::uint8_t blue = 100;

            float center_x = window_config.width_px / 2.0f;
            float center_y = window_config.height_px / 2.0f;
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