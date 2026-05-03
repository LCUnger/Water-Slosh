#pragma once

#include "Vec.h"

namespace toolbox
{
template<typename T, int Dimensions>
struct CellCentered {
 inline static constexpr Vec<T, Dimensions> offset{ static_cast<T>(0.5) };
 inline static constexpr Vec<int, Dimensions> index_extend{ 0 };

};

template<typename T, int Dimensions, int Axis>
struct FaceCentered {
	static_assert(Axis >= 0 && Axis < Dimensions, "axis out of range");
	
    inline static constexpr Vec<T, Dimensions> offset = [] {
		Vec<T, Dimensions> value{ static_cast<T>(0.5) };
		value[Axis] = static_cast<T>(0);
		return value;
	}();

	inline static constexpr Vec<int, Dimensions> index_extend = [] {
		Vec<int, Dimensions> value{ 0 };
		value[Axis] = 1;
		return value;
		}();
};
}