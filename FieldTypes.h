#pragma once

#include "Vec.h"

struct CellCentered {
	template<typename T, int Dimensions>
    inline static constexpr Vec<T, Dimensions> offset = [] {
		Vec<T, Dimensions> value{};
		for (auto& element : value) {
			element = static_cast<T>(0.5);
		}
		return value;
	}();
};

template<typename T, int Axis>
struct FaceCentered {
	template<int Dimensions>
    inline static constexpr Vec<T, Dimensions> offset = [] {
		static_assert(Axis >= 0 && Axis < Dimensions, "axis out of range");
		Vec<T, Dimensions> value{};
		for (auto& element : value) {
			element = static_cast<T>(0.5);
		}
		value[Axis] = static_cast<T>(0);
		return value;
	}();
};