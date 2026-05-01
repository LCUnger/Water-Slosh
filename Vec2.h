#pragma once

#include <array>
#include <cmath>
#include <type_traits>


template<typename T>
class Vec2
{
public:

	Vec2() : e{ 0, 0 } {}
	Vec2(T x, T y) : e{ x, y } {}


	T x() const { return e[0]; }
	T y() const { return e[1]; }

	Vec2 operator-() const { return Vec2(-e[0], -e[1]); }

	T operator[](int i) const { return e[i]; }
	T& operator[](int i) { return e[i]; }

	Vec2& operator+=(const Vec2& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		return *this;
	}

	Vec2& operator-=(const Vec2& v)
	{
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		return *this;
	}

	Vec2& operator*=(const T t)
	{
		e[0] *= t;
		e[1] *= t;
		return *this;
	}

	Vec2& operator/=(const T t)
	{
		return *this *= 1 / t;
	}

	T length_squared() const requires std::floating_point<T> {
		return e[0] * e[0] + e[1] * e[1];
	}

	T length() const requires std::floating_point<T>
	{
		return std::sqrt(length_squared());
	}

	Vec2& normalize() requires std::floating_point<T>
	{
		T len = length();
		if (len > 0) {
			return *this /= len;
		}
		return *this;

	}

	Vec2 normalized() const requires std::floating_point<T>
	{
		T len = length();
		if (len > 0) {
			return *this / len;
		}
		return *this;
	}

	bool near_zero() const requires std::floating_point<T>
	{
		const T s = static_cast<T>(1e-8);
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s);
	}


private:
	std::array<T, 2> e{};
};

