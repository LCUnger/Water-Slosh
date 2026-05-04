#pragma once

#include "Point2.h"
#include "Vec2.h"
#include "PointVecOps.h"

class Particle
{
public:
    Particle() : position_{}, velocity_{} {}
    Particle(float position_x, float position_y, float velocity_x, float velocity_y) : position_{ position_x, position_y }, velocity_{ velocity_x, velocity_y } {}


    /* Integrate / update the particle's position and velocity over time dt, given an acceleration.
    Using semi-implicit Euler integration method. */
    void integrate(float dt, const Vec2& acceleration)
    {	
        velocity_ += acceleration * dt;
        position_ += velocity_ * dt;
    }


private:
    Point2 position_{};
    Vec2 velocity_{};
};