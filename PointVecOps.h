#pragma once

#include "Point.h"
#include "Vec.h"

namespace toolbox
{
template<typename T, int dimensions>
inline Point<T, dimensions> operator+(const Point<T, dimensions>& point, const Vec<T, dimensions>& vector)
{
    Point<T, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = point[index] + vector[index];
    }
    return result;
}

template<typename T, int dimensions>
inline Point<T, dimensions> operator-(const Point<T, dimensions>& point, const Vec<T, dimensions>& vector)
{
    Point<T, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = point[index] - vector[index];
    }
    return result;
}

template<typename T, int dimensions>
inline Vec<T, dimensions> operator-(const Point<T, dimensions>& left, const Point<T, dimensions>& right)
{
    Vec<T, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = left[index] - right[index];
    }
    return result;
}
}
