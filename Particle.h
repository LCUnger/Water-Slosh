#pragma once

#include <numbers>
#include <cmath>


#include "Point2.h"
#include "Vec2.h"
#include "PointVecOps.h"


class Particle
{
public:
    Particle() : position_{}, velocity_{}, radius_{}, mass_{} {}
    Particle(double position_x, double position_y, double velocity_x, double velocity_y, double radius) : position_{ position_x, position_y }, velocity_{ velocity_x, velocity_y }, radius_{ radius }, mass_{ 4.0 / 3.0 * std::numbers::pi * std::pow(radius, 3) * 997 } {}
    Particle(const Point2& position, const Vec2& velocity, double radius) : position_(position), velocity_(velocity), radius_(radius), mass_(4.0 / 3.0 * std::numbers::pi * std::pow(radius, 3) * 997) {}

    /** 
    Integrate/update the particle's position and velocity over time dt, given an acceleration.
    Using semi-implicit Euler integration method. 
    */
    void integrate(double dt, const Vec2& acceleration, const Vec2& external_forces = Vec2{0, 0})
    {    
        Vec2 total_acceleration = acceleration;
        if (mass_ > 0.0) {
            total_acceleration += external_forces / mass_;
        }

        velocity_ += total_acceleration * dt;
        position_ += velocity_ * dt;
    }

    void update(double dt, const Vec2& acceleration, const Vec2& external_forces = Vec2{0,0})
    {
        integrate(dt, acceleration, external_forces);
    }

    Point2& position() { return position_; }
    const Point2& position() const { return position_; }

    Vec2& velocity() { return velocity_; }
    const Vec2& velocity() const { return velocity_; }

    double radius() const { return radius_; }
	double mass() const { return mass_; }


private:
    Point2 position_{};
    Vec2 velocity_{};
    double radius_{};
    double mass_{};
};
