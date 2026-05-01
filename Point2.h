#pragma once

#include "Vec2.h"

template<typename T>
class Point2
{
public:
	Point2() : e{ 0, 0 } {}
	Point2(T x, T y) : e{ x, y } {}
	
	T x() const { return e[0]; }
	T y() const { return e[1]; }

	T operator[](int i) const { return e[i]; }
	T& operator[](int i) { return e[i]; }

	Point2<T>& operator+=(const Vec2<T>& v)
	{
		e[0] += v.x();
		e[1] += v.y();
		return *this;
	}

private:
	std::array<T, 2> e{};
};

