#pragma once

#include "Vec.h"
#include "Point.h"
#include "PointVecOps.h"

namespace toolbox
{
template<typename T, int Dimensions, typename FieldType>
class ScalarField
{
public:
	

    T sample(const Point<T, Dimensions>& world_position) const {
		auto field_position = world_position - FieldType::template offset<T, Dimensions>;


	}


private:
};
}