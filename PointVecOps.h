#pragma once

#include "Point.h"
#include "Vec.h"

template<typename T, int Dimensions>
inline Point<T, Dimensions> operator+(const Point<T, Dimensions>& p, const Vec<T, Dimensions>& v)
{
    Point<T, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = p[i] + v[i];
    }
    return result;
}

template<typename T, int Dimensions>
inline Point<T, Dimensions> operator-(const Point<T, Dimensions>& p, const Vec<T, Dimensions>& v)
{
    Point<T, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = p[i] - v[i];
    }
    return result;
}

template<typename T, int Dimensions>
inline Vec<T, Dimensions> operator-(const Point<T, Dimensions>& p1, const Point<T, Dimensions>& p2)
{
    Vec<T, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = p1[i] - p2[i];
    }
    return result;
}
