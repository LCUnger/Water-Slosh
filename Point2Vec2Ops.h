#pragma once

#include "Vec2.h"
#include "Point2.h"

template<typename T, typename U>
inline Point2<std::common_type_t<T, U>> operator+(const Point2<T>& p, const Vec2<U>& v) {
	return Point2<std::common_type_t<T, U>>(p.x() + v.x(), p.y() + v.y());
}

template<typename T, typename U>
inline Point2<std::common_type_t<T, U>> operator-(const Point2<T>& p, const Vec2<U>& v) {
	return Point2<std::common_type_t<T, U>>(p.x() - v.x(), p.y() - v.y());
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator-(const Point2<T>& p1, const Point2<U>& p2) {
	return Vec2<std::common_type_t<T, U>>(p1.x() - p2.x(), p1.y() - p2.y());
}