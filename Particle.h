#pragma once

#include "Point2.h"
#include "Vec2.h"
#include "PointVecOps.h"


class Particle
{
public:
    Particle() : position_{}, velocity_{}, radius_{}, mass_{} {}
    Particle(float position_x, float position_y, float velocity_x, float velocity_y, double radius, double mass) : position_{ position_x, position_y }, velocity_{ velocity_x, velocity_y }, radius_{ radius }, mass_{ mass } {}
	Particle(const Point2& position, const Vec2& velocity, double radius, double mass) : position_(position), velocity_(velocity), radius_(radius), mass_(mass) {}

    /** 
    Integrate/update the particle's position and velocity over time dt, given an acceleration.
    Using semi-implicit Euler integration method. 
    */
    void integrate(float dt, const Vec2& acceleration, const Vec2& external_forces = Vec2{0, 0})
    {	
        velocity_ += (acceleration + external_forces / mass_) * dt;
        position_ += velocity_ * dt;
    }

    void update(float dt, const Vec2& acceleration)
    {
        integrate(dt, acceleration);
	}

	Point2& position() { return position_; }
	const Point2& position() const { return position_; }

	Vec2& velocity() { return velocity_; }
    const Vec2& velocity() const { return velocity_; }


private:
    Point2 position_{};
    Vec2 velocity_{};
    double radius_{};
	double mass_{};
};