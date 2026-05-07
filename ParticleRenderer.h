#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Particle.h"

struct RenderConfig
{
    float particle_radius_px = 2.0f;
    float pixels_per_meter = 100.0f;
};

class ParticleRenderer
{
public:
    explicit ParticleRenderer(const RenderConfig& config)
        : ParticleRenderer(config.particle_radius_px, config.pixels_per_meter)
    {
    }

    explicit ParticleRenderer(float radius_px = 2.0f, float pixels_per_meter = 1.0f)
        : radius_px_(radius_px), pixels_per_meter_(pixels_per_meter)
    {
        circle_.setRadius(radius_px_);
        circle_.setOrigin({ radius_px_, radius_px_ });
        circle_.setFillColor(color_);
    }


    void draw(sf::RenderTarget& target, const std::vector<Particle>& particles)
    {
        const float target_height = static_cast<float>(target.getSize().y);

        for (const auto& particle : particles) 
        {
            circle_.setPosition(world_to_screen(particle.position(), target_height));
            target.draw(circle_);
        }
    }

private:
    sf::Vector2f world_to_screen(const Point2& world_position, float target_height) const
    {
        return {
            static_cast<float>(world_position[0] * pixels_per_meter_),
            target_height - static_cast<float>(world_position[1] * pixels_per_meter_)
        };
    }

    float radius_px_{};
    float pixels_per_meter_{};
    sf::Color color_ = sf::Color::Blue;
    sf::CircleShape circle_{};
};
