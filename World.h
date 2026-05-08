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
    double particle_radius_m = 0.02;

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
          fluid_grid_(config.grid_width, config.grid_height, config.cell_size_m),
          particle_radius_m_(config.particle_radius_m)
    {
        fluid_grid_.initialize();
    }

    explicit World(std::size_t num_particles, std::size_t grid_width, std::size_t grid_height, double cell_size_m, double particle_radius_m)
        : World(SimulationConfig{ num_particles, grid_width, grid_height, cell_size_m, particle_radius_m })
    {
    }

    void update(float dt)
    {
        for (auto& particle : particles_) {
            particle.update(dt, gravity);
			resolveParticleCollision(particle);
        }


        
        // TODO: transfer velocity from particles to grid

        // enforce incompressibility on grid

        // transfer velocity from grid to particles
    }

    void add_particle(const Particle& particle)
    {
        particles_.push_back(particle);
    }

    const SimulationConfig& config() const { return config_; }

    std::vector<Particle>& particles() { return particles_; }
    const std::vector<Particle>& particles() const { return particles_; }

    FluidGrid<double>& fluid_grid() { return fluid_grid_; }
    const FluidGrid<double>& fluid_grid() const { return fluid_grid_; }

private:
    SimulationConfig config_;
    std::vector<Particle> particles_;
    FluidGrid<double> fluid_grid_;
    double particle_radius_m_ = 0.02;

    static constexpr Vec2 gravity{ 0 , -9.81 };


    // Currently just simple reflection at boundaries
    void resolveParticleCollision(Particle& particle)
    /**
    TODO : Better collision and spacial system for particles and solids. Options in order of increasing complexity:
    Now:
        use cell_type_ for walls and simple blockers

    Next:
      introduce Collider / SolidGeometry interface
      cell_type_ is generated from colliders

    Later:
      add SDF for smooth collision normals and particle pushout

    Much later:
      fractional/cut-cell boundaries for better pressure solve near curved solids

    */
    {
        double x_min = particle.radius();
        double x_max = config_.domain_width_m() - particle.radius();
        double y_min = particle.radius();
        double y_max = config_.domain_height_m() - particle.radius();


        if (particle.position()[0] < x_min) {
            particle.position()[0] = x_min;
            particle.velocity()[0] *= -1;
        }
        else if (particle.position()[0] > x_max) {
            particle.position()[0] = x_max;
            particle.velocity()[0] *= -1;
        }
        if (particle.position()[1] < y_min) {
            particle.position()[1] = y_min;
            particle.velocity()[1] *= -1;
        }
        else if (particle.position()[1] > y_max) {
            particle.position()[1] = y_max;
            particle.velocity()[1] *= -1;
        }
    }
};

