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
public:
    explicit FluidGrid(std::size_t width, std::size_t height, T cell_size)
        : field_width_(width), field_height_(height), cell_size_(cell_size),
          u_(ShapeType{ width, height }, cell_size, std::size_t{ 1 }),
          v_(ShapeType{ width, height }, cell_size, std::size_t{ 1 }),
          pressure_(ShapeType{ width, height }, cell_size, std::size_t{ 1 }),
          density_(ShapeType{ width, height }, cell_size, std::size_t{ 1 }),
          cell_type_(ShapeType{ width, height })
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

    }


    void transferVelocityParticleToGrid()
    {
     
    }


    void transferVelocityGridToParticle(Particle& particle)
    {
        particle.velocity() = Vec2{ u_.sample(particle.position()), v_.sample(particle.position()) };
    }
    

private:
    using ShapeType = Vec<std::size_t, 2>;

    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 0>> u_; // Velocity component in x-direction
    ScalarField<T, 2, fieldtypes::FaceCentered<T, 2, 1>> v_; // Velocity component in y-direction
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> pressure_;
    ScalarField<T, 2, fieldtypes::CellCentered<T, 2>> density_;
    ScalarField<int, 2, fieldtypes::CellCentered<int, 2>> cell_type_;

    std::size_t field_width_;
    std::size_t field_height_;
    T cell_size_;

    // TODO : Add overrelaxation
    bool forceIncompressibilityAtCell(std::size_t idx_x, std::size_t idx_y)
    {
        T divergence = u_(idx_x + 1, idx_y) - u_(idx_x, idx_y) + v_(idx_x, idx_y + 1) - v_(idx_x, idx_y);

        //HACK : find a good name for this variable.
        int s = cell_type_(idx_x + 1, idx_y) + cell_type_(idx_x - 1, idx_y) + cell_type_(idx_x, idx_y + 1) + cell_type_(idx_x, idx_y - 1);

        if (s == 0) return false; // All cells around are solid, skip

        u_(idx_x, idx_y) += divergence * (cell_type_(idx_x - 1, idx_y) / s);
        u_(idx_x + 1, idx_y) +=  -divergence * (cell_type_(idx_x + 1, idx_y) / s);
        v_(idx_x, idx_y) += divergence * (cell_type_(idx_x, idx_y - 1) / s);
        v_(idx_x, idx_y + 1) += -divergence * (cell_type_(idx_x, idx_y + 1) / s);

        return true;
    }

};

