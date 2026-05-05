#pragma once

#include "Point.h"
#include "Vec.h"

#include <cstddef>

namespace toolbox
{
template<typename T, std::size_t dimensions>
inline Point<T, dimensions> operator+(const Point<T, dimensions>& point, const Vec<T, dimensions>& vector)
{
    Point<T, dimensions> result;
    for (std::size_t index = 0; index < dimensions; ++index) {
        result[index] = point[index] + vector[index];
    }
    return result;
}

template<typename T, std::size_t dimensions>
inline Point<T, dimensions> operator-(const Point<T, dimensions>& point, const Vec<T, dimensions>& vector)
{
    Point<T, dimensions> result;
    for (std::size_t index = 0; index < dimensions; ++index) {
        result[index] = point[index] - vector[index];
    }
    return result;
}

template<typename T, std::size_t dimensions>
inline Vec<T, dimensions> operator-(const Point<T, dimensions>& left, const Point<T, dimensions>& right)
{
    Vec<T, dimensions> result;
    for (std::size_t index = 0; index < dimensions; ++index) {
        result[index] = left[index] - right[index];
    }
    return result;
}
}
