#pragma once

#include "Vec.h"

namespace toolbox
{
template<typename T, int dimensions>
struct CellCentered {
 inline static constexpr Vec<T, dimensions> offset{ static_cast<T>(0.5) };
 inline static constexpr Vec<int, dimensions> index_extend{};

};

template<typename T, int dimensions, int axis>
struct FaceCentered {
 static_assert(axis >= 0 && axis < dimensions, "axis out of range");
	
    inline static constexpr Vec<T, dimensions> offset = [] {
        Vec<T, dimensions> value{ static_cast<T>(0.5) };
        value[axis] = static_cast<T>(0);
		return value;
	}();

    inline static constexpr Vec<int, dimensions> index_extend = [] {
        Vec<int, dimensions> value{ 0 };
        value[axis] = 1;
		return value;
		}();
};
}