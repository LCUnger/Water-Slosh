#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Particle.h"


class ParticleRenderer
{
public:
	ParticleRenderer(float radius = 2.0f, float pixels_per_unit = 1.0f)
		: radius_(radius), pixels_per_unit_(pixels_per_unit)
	{
		circle_.setRadius(radius_);
		circle_.setOrigin({ radius_, radius_ });
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
			static_cast<float>(world_position[0] * pixels_per_unit_),
			target_height - static_cast<float>(world_position[1] * pixels_per_unit_)
		};
	}

	float radius_{};
	float pixels_per_unit_{};
	sf::Color color_ = sf::Color::Blue;
	sf::CircleShape circle_{};
};
