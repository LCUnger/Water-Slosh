#pragma once

#include "Vec.h"
#include "Point.h"
#include "PointVecOps.h"
#include "Tensor.h"

namespace toolbox
{
template<typename T, std::size_t dimensions, typename FieldType, std::size_t... Extents>
class ScalarField
{
	template<typename IndexSequence>
	struct DataTypeHelper;

	template<std::size_t... Is>
	struct DataTypeHelper<std::index_sequence<Is...>>
	{
		using type = Tensor<T, dimensions, (Extents + static_cast<std::size_t>(FieldType::index_extend[Is]))...>;
	};

	using DataType = typename DataTypeHelper<std::make_index_sequence<dimensions>>::type;


	// Other solution, less complicated but less elegant:
	//template<std::size_t... Is>
	//static Tensor<T, dimensions, (Extents + FieldType::index_extend[Is]) ...> make_data_type(std::index_sequence<Is...>);

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
  DataType data_;

	T interpolate(const Point<T, dimensions>& field_position) const
	{
		(void)field_position;
		return T{};
	}

};
}