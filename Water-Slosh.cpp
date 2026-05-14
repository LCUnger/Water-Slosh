#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <cstddef>

#include "ParticleRenderer.h"
#include "World.h"

struct WindowConfig
{
    unsigned int width_px = 1280;
    unsigned int height_px = 900;
};

void seedLowerHalfFluid(World& world)
{
    const auto& config = world.config();
    const double radius = config.particle_radius_m;
    const double spacing = radius * 3.0;
    const double x_min = radius * 2.0;
    const double x_max = config.domain_width_m() - radius * 2.0;
    const double y_min = radius * 2.0;
    const double y_max = config.domain_height_m() * 0.5;

    std::size_t particles_added = 0;
    for (double y = y_min; y < y_max && particles_added < config.num_particles; y += spacing) {
        for (double x = x_min; x < x_max && particles_added < config.num_particles; x += spacing) {
            world.add_particle(Particle(x, y, 0.0, 0.0, radius));
            ++particles_added;
        }
    }
}

int main()
{
    const SimulationConfig simulation_config{
        .num_particles = 2000,
        // Simulation cells, not screen pixels. 160x120 is already 19,200 cells.
        .grid_width = 160,
        .grid_height = 120,
        .cell_size_m = 0.05,
        .particle_radius_m = 0.035
    };


    World world(simulation_config);

    const WindowConfig window_config;

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

    seedLowerHalfFluid(world);

    sf::Clock clock;

    while (window.isOpen())
    {
		const float dt = std::min(clock.restart().asSeconds(), 0.033f); // Cap at roughly 30 FPS to avoid instability in the simulation when the window is not focused

        world.update(static_cast<double>(dt));
        
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
