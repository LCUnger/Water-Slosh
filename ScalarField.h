#pragma once

#include "Vec.h"
#include "Point.h"
#include "PointVecOps.h"
#include "ArrayND.h"

namespace toolbox
{
template<typename T, std::size_t dimensions, typename FieldType>
class ScalarField
{
	using DataType = ArrayND<T, dimensions>;
	using ShapeType = typename DataType::ShapeType;
	using PointType = Point<T, dimensions>;

public:
	ScalarField() = default;

	ScalarField(const ShapeType& shape, T cell_size = 1, std::size_t ghost_width = 1)
		: cell_size_(cell_size), ghost_width_(std::max(ghost_width, std::size_t(1))), data_(shape + FieldType::index_extend, ghost_width_) {}

	ScalarField(const ShapeType& shape, const T& initial_value, T cell_size = 1, std::size_t ghost_width = 1)
		: cell_size_(cell_size), ghost_width_(std::max(ghost_width, std::size_t(1))), data_(shape, initial_value, ghost_width_) {}

	//using DataType = decltype(make_data_type(std::make_index_sequence<dimensions>{}));


public:
	

    T sample(const Point<T, dimensions>& world_position) const {
     const auto field_position = world_position - FieldType::offset;
		(void)field_position;
		return T{};
	}

	Point<T, dimensions> worldToField(const Point<T, dimensions>& world_position) const
	{
		return world_position - FieldType::offset;
	}

	Point<T, dimensions> fieldToWorld(const Point<T, dimensions>& field_position) const
	{
		return field_position + FieldType::offset;
	}


private:
	T cell_size_{ 1 };
	std::size_t ghost_width_{ 1 };
  DataType data_;

	T interpolate(const Point<T, dimensions>& field_position) const
	{
		(void)field_position;
		return T{};
	}

};
}