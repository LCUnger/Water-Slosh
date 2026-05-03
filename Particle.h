#pragma once

#include "Point2.h"
#include "Vec2.h"
#include "PointVecOps.h"

class Particle
{
public:
    Particle() : position{}, velocity{} {}
    Particle(float position_x, float position_y, float velocity_x, float velocity_y) : position{ position_x, position_y }, velocity{ velocity_x, velocity_y } {}


    /* Integrate / update the particle's position and velocity over time dt, given an acceleration.
    Using semi-implicit Euler integration method. */
    void integrate(float dt, const Vec2& acceleration)
    {	
        velocity += acceleration * dt;
        position += velocity * dt;
    }


private:
    Point2 position{};
    Vec2 velocity{};
};