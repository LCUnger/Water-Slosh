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

public:
	ScalarField() = default;

	explicit ScalarField(const ShapeType& shape, T cell_size = 1, std::size_t ghost_width = 1)
		: cell_size_(cell_size),
		ghost_width_(std::max(ghost_width, std::size_t(1))),
		data_(shape + FieldType::index_extend + ShapeType(ghost_width_)) {}

	explicit ScalarField(const ShapeType& shape, const T& initial_value, T cell_size = 1, std::size_t ghost_width = 1)
		: cell_size_(cell_size),
		ghost_width_(std::max(ghost_width, std::size_t(1))),
		data_(shape + FieldType::index_extend + ShapeType(ghost_width_), initial_value) {}


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
		return data_[physicalToDataIndex(IndexType{ static_cast<int>(indices)... })];
	}

	template<typename... Indices>
       requires (sizeof...(Indices) == dimensions)
	const T& operator()(Indices... indices) const
	{
		return data_[physicalToDataIndex(IndexType{ static_cast<int>(indices)... })];
	}
	
	T& operator()(const IndexType& index)
	{
		return data_[physicalToDataIndex(index)];
	}

	const T& operator()(const IndexType& index) const
	{
		return data_[physicalToDataIndex(index)];
	}

	PointType worldToField(const PointType& world_position) const
	{
		return world_position - FieldType::offset;
	}

	PointType fieldToWorld(const PointType& field_position) const
	{
		return field_position + FieldType::offset;
	}
	//TODO : add sampling method with interpolation
	T sample(const PointType& world_position) const {
		const auto field_position = world_position - FieldType::offset;
		(void)field_position;
		return T{};
	}


private:
	T cell_size_{ 1 };
	std::size_t ghost_width_{ 1 };
  DataType data_;

	
	IndexType physicalToDataIndex(const IndexType& physical_index) const
	{
		return physical_index + IndexType(static_cast<int>(ghost_width_));
	}


	// TODO : add interpolation method
	T interpolateLinear(const Point<T, dimensions>& field_position) const
	{

		std::array<T, std::pow(2, dimensions)> weights{};

		static constexpr std::size_t stencil_size = std::size_t{ 1 } << dimensions;
		std::array<T, stencil_size> weights{};
		for 

		return T{};
	}

};
}