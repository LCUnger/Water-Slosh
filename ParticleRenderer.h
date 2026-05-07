#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Particle.h"
#include "RenderTransform.h"

struct RenderConfig
{
    float particle_radius_px = 2.0f;
};

class ParticleRenderer
{
public:
    explicit ParticleRenderer(const RenderConfig& config)
        : ParticleRenderer(config.particle_radius_px)
    {
    }

    explicit ParticleRenderer(float radius_px = 2.0f)
        : radius_px_(radius_px)
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
    float radius_px_{};
    sf::Color color_ = sf::Color::Blue;
    sf::CircleShape circle_{};
};
