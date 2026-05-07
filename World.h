#pragma once

#include <cstddef>
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
	explicit World(const SimulationConfig& config)
		: config_(config),
		  particles_(config.num_particles),
		  fluid_grid_(config.grid_width, config.grid_height, config.cell_size_m)
	{
		fluid_grid_.initialize();
	}

	explicit World(std::size_t num_particles, std::size_t grid_width, std::size_t grid_height, double cell_size_m)
		: World(SimulationConfig{ num_particles, grid_width, grid_height, cell_size_m })
	{
	}

	void update(float dt) {}

private:
	std::vector<Particle> particles_;
	FluidGrid<double> fluid_grid_;


};

