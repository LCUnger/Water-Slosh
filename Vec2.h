#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <concepts>


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


template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec2<T>& v) {
	return out << v.x() << ' ' << v.y() << ' ';
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator+(const Vec2<T>& u, const Vec2<U>& v) {
	return Vec2<std::common_type_t<T, U>>(u.x() + v.x(), u.y() + v.y());
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator-(const Vec2<T>& u, const Vec2<U>& v) {
	return Vec2<std::common_type_t<T, U>>(u.x() - v.x(), u.y() - v.y());
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator*(const Vec2<T>& u, const Vec2<U>& v) {
	return Vec2<std::common_type_t<T, U>>(u.x() * v.x(), u.y() * v.y());
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator*(const Vec2<T>& v, U t) {
	return Vec2<std::common_type_t<T, U>>(v.x() * t, v.y() * t);
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator*(U t, const Vec2<T>& v) {
	return Vec2<std::common_type_t<T, U>>(v.x() * t, v.y() * t);
}

template<typename T, typename U>
inline Vec2<std::common_type_t<T, U>> operator/(const Vec2<T>& v, U t) {
	return Vec2<std::common_type_t<T, U>>(v.x() / t, v.y() / t);
}


template<typename T, typename U>
inline std::common_type_t<T, U> dot(const Vec2<T>& u, const Vec2<U>& v) {
	return u.x() * v.x() + u.y() * v.y();
}

template<typename T>
inline Vec2<T> unit_vector(const Vec2<T>& v) {
	return v / v.length();
}