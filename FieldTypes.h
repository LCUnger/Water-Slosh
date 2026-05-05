#pragma once

#include "Vec.h"
#include <array>

namespace toolbox
{
namespace fieldtypes
{
template<typename T, std::size_t dimensions>
struct CellCentered {
    inline static constexpr Vec<T, dimensions> offset{ static_cast<T>(0.5) };
    inline static constexpr Vec<std::size_t, dimensions> index_extend{ 0 };

};

template<typename T, std::size_t dimensions, std::size_t axis>
struct FaceCentered {
    static_assert(axis >= 0 && axis < dimensions, "axis out of range");

    inline static constexpr Vec<T, dimensions> offset = [] {
        Vec<T, dimensions> value{ static_cast<T>(0.5) };
        value[axis] = static_cast<T>(0);
        return value;
        }();

    inline static constexpr Vec<std::size_t, dimensions> index_extend = [] {
        Vec<std::size_t, dimensions> value{ 0 };
        value[axis] = 1;
        return value;
        }();
};
}
}