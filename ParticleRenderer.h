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

		
	}

private:

	float radius_{};
	float pixels_per_unit_{};
	sf::Color color_ = sf::Color::Blue;
	sf::CircleShape circle_{};
};
