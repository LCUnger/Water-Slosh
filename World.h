#pragma once

#include <vector>

#include "Particle.h"
#include "FluidGrid.h"

class World
{
public:
	World(size_t num_particles, size_t grid_width, size_t grid_height)
		: particles_(num_particles), fluid_grid_(grid_width, grid_height)
	{
		fluid_grid_.initialize();
	}


	void update(float dt) {}

private:
	std::vector<Particle> particles_;
	FluidGrid<double> fluid_grid_;


};

