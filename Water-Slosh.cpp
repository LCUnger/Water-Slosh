#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>

#include "ParticleRenderer.h"
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
        .grid_width = 800,
        .grid_height = 600,
        .cell_size_m = 0.05,
        .particle_radius_m = 0.4
    };


    World world(simulation_config);


    sf::RenderWindow window(sf::VideoMode({ window_config.width_px, window_config.height_px }), "SFML 3 Test Render");

    const RenderTransform render_transform = RenderTransform::fit_to_target(
        world.config().domain_width_m(),
        world.config().domain_height_m(),
        window.getSize()
    );

    const RenderConfig render_config{
        .transform = render_transform,
        .particle_radius_m = static_cast<float>(world.config().particle_radius_m)
    };

    ParticleRenderer particle_renderer(render_config);

    world.add_particle(Particle(
        (world.config().domain_width_m() / 2.0),
        (world.config().domain_height_m() / 2.0),
        0.3,
        1.0,
        world.config().particle_radius_m));

    while (window.isOpen())
    {
        world.update(0.004);
        
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        particle_renderer.draw(window, world.particles(), render_transform);
        window.display();
    }
}
