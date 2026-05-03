#pragma once

#include "Vec.h"
#include "Point.h"
#include "PointVecOps.h"

namespace toolbox
{
template<typename T, int dimensions, typename FieldType>
class ScalarField
{
public:
	

    T sample(const Point<T, dimensions>& world_position) const {
     const auto field_position = world_position - FieldType::offset;
		(void)field_position;
		return T{};
	}


private:


};
}