#pragma once

#include "Vec.h"
#include "Point.h"
#include "PointVecOps.h"
#include "ArrayND.h"

#include <algorithm>
#include <array>

namespace toolbox
{
template<typename T, std::size_t dimensions, typename FieldType>
class ScalarField
{
    using DataType = ArrayND<T, dimensions>;
    using ShapeType = Vec<std::size_t, dimensions>;
    using IndexType = Vec<int, dimensions>;
    using PointType = Point<T, dimensions>;
    using StencilType = std::array<IndexType, std::size_t(1) << dimensions>;
    using WeightsType = std::array<T, std::size_t(1) << dimensions>;

public:
    ScalarField() = default;
    
    ScalarField(const ShapeType& shape, T cell_size = 1, std::size_t ghost_width = 1)
        : cell_size_(cell_size),
        ghost_width_(std::max(ghost_width, std::size_t(1))),
        physical_shape_(shape + FieldType::index_extend),
        data_(physical_shape_ + ShapeType(2 * ghost_width_)) {}

    ScalarField(const ShapeType& shape, const T& initial_value, T cell_size = 1, std::size_t ghost_width = 1)
        : cell_size_(cell_size),
        ghost_width_(std::max(ghost_width, std::size_t(1))),
        physical_shape_(shape + FieldType::index_extend),
        data_(physical_shape_ + ShapeType(2 * ghost_width_), initial_value) {}


    DataType& data() { return data_; }
    const DataType& data() const { return data_; }

    T& at(const IndexType& index)
    {
        return data_.at(physicalToDataIndex(index));
    }

    const T& at(const IndexType& index) const
    {
        return data_.at(physicalToDataIndex(index));
    }

    template<typename... Indices>
       requires (sizeof...(Indices) == dimensions)
    T& at(Indices... indices)
    {
        return at(IndexType{ static_cast<int>(indices)... });
    }

    template<typename... Indices>
       requires (sizeof...(Indices) == dimensions)
    const T& at(Indices... indices) const
    {
        return at(IndexType{ static_cast<int>(indices)... });
    }

    template<typename... Indices>
       requires (sizeof...(Indices) == dimensions)
    T& operator()(Indices... indices)
    {
        return data_(physicalToDataIndex(IndexType{ static_cast<int>(indices)... }));
    }

    template<typename... Indices>
       requires (sizeof...(Indices) == dimensions)
    const T& operator()(Indices... indices) const
    {
        return data_(physicalToDataIndex(IndexType{ static_cast<int>(indices)... }));
    }

    T& operator()(const IndexType& index)
    {
        return data_(physicalToDataIndex(index));
    }

    const T& operator()(const IndexType& index) const
    {
        return data_(physicalToDataIndex(index));
    }

    PointType worldToField(const PointType& world_position) const
    {
        return world_position - FieldType::offset;
    }

    PointType fieldToWorld(const PointType& field_position) const
    {
        return field_position + FieldType::offset;
    }

    bool isStored(const IndexType& index) const
    {
        if (data_.empty()) {
            return false;
        }

        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            const int lower_bound = -static_cast<int>(ghost_width_);
            if (index[axis] < lower_bound) {
                return false;
            }

            const auto data_index = static_cast<std::size_t>(index[axis] - lower_bound);
            if (data_index >= data_.shape()[axis]) {
                return false;
            }
        }
        return true;
    }

    bool isPhysical(const IndexType& index) const
    {
        if (data_.empty()) {
            return false;
        }

        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            if (index[axis] < 0 || static_cast<std::size_t>(index[axis]) >= physical_shape_[axis]) {
                return false;
            }
        }
        return true;
    }

    IndexType positionToIndex(const PointType& world_position) const {
        const PointType field_position = worldToField(world_position);
        IndexType index;
        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            index[axis] = static_cast<int>(std::floor(field_position[axis] / cell_size_));
        }
        return index;
    }

    PointType positionToCellposition(const PointType& world_position) const {
        const PointType field_position = worldToField(world_position);
        PointType cell_position;
        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            cell_position[axis] = field_position[axis] - cell_size_ * std::floor(field_position[axis] / cell_size_);
        }
        return cell_position;
    }

    static constexpr StencilType StencilOffsets() {
        return stencil_offsets_;
    }

    StencilType interpolationStencil(const PointType& world_position) const
    {
        constexpr std::size_t num_stencil_points = std::size_t(1) << dimensions;
        const StencilType stencil_offsets = StencilOffsets();

        StencilType stencil;
        for (std::size_t i = 0; i < num_stencil_points; ++i) {
            stencil[i] = positionToIndex(world_position) + stencil_offsets[i];
        }
        return stencil;
    }

    WeightsType interpolationLinearWeights(const PointType& cell_position)
    {
        WeightsType weights{};

        StencilType stencil_offsets = StencilOffsets();

        for (int i = 0; i < weights.size(); ++i) {
            for (std::size_t axis = 0; axis < dimensions; ++axis) {
                if (stencil_offsets[i][axis] == 1) {
                    weights[i] *= (cell_position[axis] / cell_size_);
                }
                else {
                    weights[i] *= (1 - cell_position[axis] / cell_size_);
                }
            }
        }

        return weights;

    }

    T interpolateLinear(const PointType& cell_position, const StencilType& stencil_points) const
    {
        WeightsType weights =  interpolationLinearWeights(cell_position);

        T result{};
        T weight_sum{};

        for (std::size_t i = 0; i < weights.size(); ++i) {
            result += (*this)(stencil_points[i]) * weights[i];
            weight_sum += weights[i];
        }

        result /= weight_sum;

        return result;
    }

    //TODO : add sampling method with interpolation
    T sample(const PointType& world_position) const {
        StencilType stencil = interpolationStencil(world_position);
        PointType cell_position = positionToCellposition(world_position);

        // Check if all stencil points are stored in the grid, if not throw an exception (could be out of bounds or ghost cells)
        for (const auto& index : stencil) {
            if (!isStored(index)) {
                throw std::out_of_range("Attempting to sample from an unstored position. If position should be in grid: check ghost cells");
            }
        }

        T interprolated = interpolateLinear(cell_position, stencil);

        return interprolated;
    }


private:
    T cell_size_{ 1 };
    std::size_t ghost_width_{ 1 };
    ShapeType physical_shape_{};
    DataType data_;

    IndexType physicalToDataIndex(const IndexType& physical_index) const
    {
        return physical_index + IndexType(static_cast<int>(ghost_width_));
    }

    static constexpr StencilType makeStencilOffsets()
    {
        constexpr std::size_t num_stencil_points = std::size_t(1) << dimensions;
        StencilType stencil;
        for (std::size_t i = 0; i < num_stencil_points; ++i) {
            IndexType offset;
            for (std::size_t axis = 0; axis < dimensions; ++axis) {
                offset[axis] = (i & (std::size_t(1) << axis)) ? 1 : 0;
            }
            stencil[i] = offset;
        }
        return stencil;
    }

    static constexpr StencilType stencil_offsets_ = makeStencilOffsets();


    // TODO : Decide what to do regarding S field / active cells, and how to handle interpolation near boundaries (ghost cells)

};
}
