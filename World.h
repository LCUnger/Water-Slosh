#pragma once

#include <vector>

#include "Particle.h"
#include "FluidGrid.h"

struct SimulationConfig
{
	std::size_t num_particles = 1000;
	std::size_t grid_width = 100;
	std::size_t grid_height = 60;
	double cell_size_m = 0.05;

	double domain_width_m() const
	{
		return static_cast<double>(grid_width) * cell_size_m;
	}

	double domain_height_m() const
	{
		return static_cast<double>(grid_height) * cell_size_m;
	}
};

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

