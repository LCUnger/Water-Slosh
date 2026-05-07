#pragma once

#include <vector>

#include "Particle.h"
#include "FluidGrid.h"

class World
{
public:


private:
	std::vector<Particle> particles_;
	FluidGrid<double> fluid_grid_;


};

