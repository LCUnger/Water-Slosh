#pragma once

/**
MAC - grid incompressibility stencil for center cell(i, j)

                    +--------------------+
                    |                    |
                    |     s(i,j+1)       |
                    |                    |
                    |                    |
                    |v(i,j+1) ^          |
                    |         |          |
+-------------------+--------------------+-----------------------+
|                   |                    |                       |
|     s(i-1,j)      | u(i,j)             | u(i+1,j)              |
|                   |-->  cell(i,j)      |-->         s(i+1,j)   |
|                   |         s(i,j)     |                       |
|                   |                    |                       |
+-------------------+---------^----------+-----------------------+
                    |         | v(i,j)   |
                    |                    |
                    |                    |
                    |                    |
                    |     s(i,j-1)       |
                    |                    |
                    +--------------------+



Divergence for cell(i, j) :

    div(i, j) = u(i + 1, j) - u(i, j)
    + v(i, j + 1) - v(i, j)

Face meanings :

    u(i, j) = velocity on the left face of cell(i, j)
    u(i + 1, j) = velocity on the right face of cell(i, j)

    v(i, j) = velocity on the bottom face of cell(i, j)
    v(i, j + 1) = velocity on the top face of cell(i, j)

Cell - centered values :

s(i, j) = center cell value
s(i - 1, j) = left neighbor
s(i + 1, j) = right neighbor
s(i, j - 1) = bottom neighbor
s(i, j + 1) = top neighbor
*/

#include "ScalarField.h"
#include "FieldTypes.h"
#include "Particle.h"

#include <cstddef>

using namespace toolbox;

enum class CellType
{
    Solid,
    Liquid,
    // Gas = -1
};

template<typename T>
class FluidGrid
{
    using ShapeType = Vec<std::size_t, 2>;
    using IndexType = Vec<int, 2>;
    std::size_t ghost_width_ = 1;

public:
    FluidGrid(std::size_t width, std::size_t height, T cell_size)
        : field_width_(width), field_height_(height), cell_size_(cell_size),
          u_(ShapeType{ width, height }, cell_size, ghost_width_),
          v_(ShapeType{ width, height }, cell_size, ghost_width_),
          u_weight_sum_(ShapeType{ width, height }, cell_size, ghost_width_),
          v_weight_sum_(ShapeType{ width, height }, cell_size, ghost_width_),
          prev_u_(ShapeType{ width, height }, cell_size, ghost_width_),
          prev_v_(ShapeType{ width, height }, cell_size, ghost_width_),
          pressure_(ShapeType{ width, height }, cell_size, ghost_width_),
          density_(ShapeType{ width, height }, cell_size, ghost_width_),
          cell_type_(ShapeType{ width, height }, cell_size, ghost_width_)
    {
    }

    std::size_t width() const { return field_width_; }
    std::size_t height() const { return field_height_; }
    T cell_size() const { return cell_size_; }
    T physical_width() const { return static_cast<T>(field_width_) * cell_size_; }
    T physical_height() const { return static_cast<T>(field_height_) * cell_size_; }

    void initialize() {};


    void setCellTypeMask() {};


    void forceIncompressibility()
       /** HACK: Currently using Gauss Sidel, which has directional bias.This is a very naive implementation of enforcing incompressibility, we will need to solve a Poisson equation for the pressure field to get the correct velocity correction.
        Improvement options in order of increasing complexity:
        1. Gauss Sidel with red-black ordering to remove directional bias.
        2. Jacobi method to remove directional bias, but slower convergence.
        3. Conjugate gradient method to solve the Poisson equation for the pressure field, which will give us the correct velocity correction in one step, but is more complex to implement.
        Note : we can also consider using a multigrid method to solve the Poisson equation, which will give us even faster convergence, but is even more complex to implement.
        */
    {
        // TODO : Iterations
        for (std::size_t x = 0; x < field_width_; ++x) {
            for (std::size_t y = 0; y < field_height_; ++y) {
                if (!forceIncompressibilityAtCell(x, y)) continue;
            }
        }

        clearGhostCells();
    }

    void transferVelocityParticlesToGrid(const std::vector<Particle>& particles)
    {
        for (const auto& particle : particles) {
            transferVelocityParticleToGrid(particle);
        }
		normalizebyWeight();
		copyCurrentVelocityToPrevious();
	}

    void transferVelocityParticleToGrid(const Particle& particle)
    {
        IndexType cell_index = cell_type_.positionToIndex(particle.position());
        Point2 cell_position = cell_type_.positionToCellposition(particle.position());


        if (cell_type_(cell_index) == static_cast<T>(CellType::Solid)) {
            return; // Skip solid cells
        }

        auto u_stencil = u_.interpolationStencil(particle.position());
        auto v_stencil = v_.interpolationStencil(particle.position());

        auto u_weights = u_.interpolationLinearWeights(cell_position);
        auto v_weights = v_.interpolationLinearWeights(cell_position);


        for (std::size_t i = 0; i < u_stencil.size(); ++i) {
            if (u_.isStored(u_stencil[i])) {
                u_(u_stencil[i]) += particle.velocity()[0] * u_weights[i];
                u_weight_sum_(u_stencil[i]) += u_weights[i];
            }
            if (v_.isStored(v_stencil[i])) {
                v_(v_stencil[i]) += particle.velocity()[1] * v_weights[i];
                v_weight_sum_(v_stencil[i]) += v_weights[i];
            }
        }
    }

    void clearGhostCells()
    {
        const std::size_t total_width = field_width_ + 2 * ghost_width_;
        const std::size_t total_height = field_height_ + 2 * ghost_width_;

        for (std::size_t x = 0; x < total_width; ++x) {
            for (std::size_t y = 0; y < ghost_width_; ++y) {
                u_.data()(x, y) = 0;
                v_.data()(x, y) = 0;
            }

            for (std::size_t y = total_height - ghost_width_; y < total_height; ++y) {
                u_.data()(x, y) = 0;
                v_.data()(x, y) = 0;
            }
        }

        for (std::size_t y = ghost_width_; y < total_height - ghost_width_; ++y) {
            for (std::size_t x = 0; x < ghost_width_; ++x) {
                u_.data()(x, y) = 0;
                v_.data()(x, y) = 0;
            }

            for (std::size_t x = total_width - ghost_width_; x < total_width; ++x) {
                u_.data()(x, y) = 0;
                v_.data()(x, y) = 0;
            }
        }
    }


    void transferVelocityGridToParticles(std::vector<Particle>& particles, double flip_ratio)
    {
        auto u_delta = u_ - prev_u_;
        auto v_delta = v_ - prev_v_;
        for (auto& particle : particles) {
            Vec2 grid_velocity{ u_.sample(particle.position()), v_.sample(particle.position()) };
            Vec2 delta_grid_velocity{ u_delta.sample(particle.position()), v_delta.sample(particle.position()) };
            particle.velocity() = (1.0 - flip_ratio) * grid_velocity + flip_ratio * (particle.velocity() + delta_grid_velocity);
        }
    }
    

private:
    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 0>> u_; // Velocity component in x-direction
    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 1>> v_; // Velocity component in y-direction
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> u_weight_sum_; // For normalizing the velocity after transferring from particles to grid
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> v_weight_sum_; // For normalizing the velocity after transferring from particles to grid

    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 0>> prev_u_; // Previous u_ field
    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 1>> prev_v_; // Previous v_ field

    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> pressure_;
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> density_;
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> cell_type_;

    std::size_t field_width_;
    std::size_t field_height_;
    T cell_size_;

    // TODO : Add overrelaxation
    bool forceIncompressibilityAtCell(std::size_t idx_x, std::size_t idx_y)
    {
        T divergence = u_(idx_x + 1, idx_y) - u_(idx_x, idx_y) + v_(idx_x, idx_y + 1) - v_(idx_x, idx_y);

        //HACK : find a good name for this variable.
        T s = cell_type_(idx_x + 1, idx_y) + cell_type_(idx_x - 1, idx_y) + cell_type_(idx_x, idx_y + 1) + cell_type_(idx_x, idx_y - 1);

        if (s == static_cast<T>(0)) return false; // All cells around are solid, skip

        u_(idx_x, idx_y) += divergence * (cell_type_(idx_x - 1, idx_y) / s);
        u_(idx_x + 1, idx_y) +=  -divergence * (cell_type_(idx_x + 1, idx_y) / s);
        v_(idx_x, idx_y) += divergence * (cell_type_(idx_x, idx_y - 1) / s);
        v_(idx_x, idx_y + 1) += -divergence * (cell_type_(idx_x, idx_y + 1) / s);

        return true;
    }

    void normalizebyWeight()
    {
        for (std::size_t i = 0; i < field_width_ * field_height_; ++i) {
            u_.data()[i] = (u_weight_sum_.data()[i] > 0) ? u_.data()[i] / u_weight_sum_.data()[i] : u_.data()[i];
            v_.data()[i] = (v_weight_sum_.data()[i] > 0) ? v_.data()[i] / v_weight_sum_.data()[i] : v_.data()[i];
        }
    }

    void copyCurrentVelocityToPrevious()
    {
        prev_u_.data() = u_.data();
        prev_v_.data() = v_.data();
    }

};

