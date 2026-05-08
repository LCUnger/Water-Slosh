#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Particle.h"
#include "RenderTransform.h"

struct RenderConfig
{
	RenderTransform transform = RenderTransform::fit_to_target(5.0, 3.0, { 800, 600 });
    float particle_radius_m = 0.02;
};

class ParticleRenderer
{
public:
    explicit ParticleRenderer(const RenderConfig& config)
        : ParticleRenderer(config.transform, config.particle_radius_m)
    {
    }

    explicit ParticleRenderer(const RenderTransform& transform, float radius_m = 0.02f)
		: transform_(transform), radius_px_(transform.meters_to_pixels(radius_m))
    {
        circle_.setRadius(radius_px_);
        circle_.setOrigin({ radius_px_, radius_px_ });
        circle_.setFillColor(color_);
    }


    void draw(sf::RenderTarget& target, const std::vector<Particle>& particles, const RenderTransform& transform)
    {
        for (const auto& particle : particles) 
        {
            circle_.setPosition(transform.world_to_screen(particle.position()));
            target.draw(circle_);
        }
    }

private:
    RenderTransform transform_;
    float radius_px_{};
    sf::Color color_ = sf::Color::Blue;
    sf::CircleShape circle_{};
};
