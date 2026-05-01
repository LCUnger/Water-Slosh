#pragma once

#include "Point2.h"
#include "Vec2.h"
#include "Point2Vec2Ops.h"

class Particle
{
public:
	Particle() : pos(0, 0), velocity(0, 0) {}
	Particle(float x, float y, float v_x, float v_y) : pos(x, y), velocity(v_x, v_y) {}


	// Integrate/update the particle's position and velocity over time dt, given an acceleration.
	void integrate(float dt, const Vec2<float>& acceleration)
	{	
		velocity += acceleration * dt;
		pos += velocity * dt;
	}


private:
	Point2<float> pos{};
	Vec2<float> velocity{};
};