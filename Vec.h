#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>

namespace toolbox
{
template<typename T, int Dimensions>
class Vec
{
public:
    constexpr Vec() = default;

    template<typename... Args>
        requires (sizeof...(Args) == Dimensions && (std::convertible_to<Args, T> && ...))
    constexpr Vec(Args... args) : e{ static_cast<T>(args)... } {}

    constexpr explicit Vec(const std::array<T, Dimensions>& values) : e(values) {}

    constexpr T at(int i) const { return e[i]; }

    constexpr const T& operator[](int i) const { return e[i]; }
    constexpr T& operator[](int i) { return e[i]; }

    constexpr Vec operator-() const
    {
        Vec result;
        for (int i = 0; i < Dimensions; ++i) {
            result[i] = -e[i];
        }
        return result;
    }

    constexpr Vec& operator+=(const Vec& v)
    {
        for (int i = 0; i < Dimensions; ++i) {
            e[i] += v.e[i];
        }
        return *this;
    }

    constexpr Vec& operator-=(const Vec& v)
    {
        for (int i = 0; i < Dimensions; ++i) {
            e[i] -= v.e[i];
        }
        return *this;
    }

    template<typename U>
    constexpr Vec& operator*=(const U t)
    {
        for (auto& component : e) {
            component *= static_cast<T>(t);
        }
        return *this;
    }

    template<typename U>
    constexpr Vec& operator/=(const U t)
    {
        for (auto& component : e) {
            component /= static_cast<T>(t);
        }
        return *this;
    }

    T length_squared() const requires std::floating_point<T>
    {
        T sum{};
        for (const auto& component : e) {
            sum += component * component;
        }
        return sum;
    }

    T length() const requires std::floating_point<T>
    {
        return static_cast<T>(std::sqrt(length_squared()));
    }

    Vec& normalize() requires std::floating_point<T>
    {
        const T len = length();
        if (len > 0) {
            return *this /= len;
        }
        return *this;
    }

    Vec normalized() const requires std::floating_point<T>
    {
        Vec result(*this);
        result.normalize();
        return result;
    }

    bool near_zero() const requires std::floating_point<T>
    {
        const T s = static_cast<T>(1e-8);
        for (const auto& component : e) {
            if (std::fabs(component) >= s) {
                return false;
            }
        }
        return true;
    }

	Vec& fill(const T value)
    {
        for (auto& component : e) {
            component = value;
        }
        return *this;
    }

    constexpr auto begin() { return e.begin(); }
    constexpr auto end() { return e.end(); }
    constexpr auto begin() const { return e.begin(); }
    constexpr auto end() const { return e.end(); }

    constexpr auto cbegin() const { return e.cbegin(); }
    constexpr auto cend() const { return e.cend(); }

private:
    std::array<T, Dimensions> e{};
};

template<typename T, int Dimensions>
inline std::ostream& operator<<(std::ostream& out, const Vec<T, Dimensions>& v)
{
    for (int i = 0; i < Dimensions; ++i) {
        if (i > 0) {
            out << ' ';
        }
        out << v[i];
    }
    return out;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator+(const Vec<T, Dimensions>& u, const Vec<U, Dimensions>& v)
{
    Vec<std::common_type_t<T, U>, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = u[i] + v[i];
    }
    return result;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator-(const Vec<T, Dimensions>& u, const Vec<U, Dimensions>& v)
{
    Vec<std::common_type_t<T, U>, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = u[i] - v[i];
    }
    return result;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator*(const Vec<T, Dimensions>& u, const Vec<U, Dimensions>& v)
{
    Vec<std::common_type_t<T, U>, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = u[i] * v[i];
    }
    return result;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator*(const Vec<T, Dimensions>& v, U t)
{
    Vec<std::common_type_t<T, U>, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = v[i] * t;
    }
    return result;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator*(U t, const Vec<T, Dimensions>& v)
{
    return v * t;
}

template<typename T, typename U, int Dimensions>
inline Vec<std::common_type_t<T, U>, Dimensions> operator/(const Vec<T, Dimensions>& v, U t)
{
    Vec<std::common_type_t<T, U>, Dimensions> result;
    for (int i = 0; i < Dimensions; ++i) {
        result[i] = v[i] / t;
    }
    return result;
}

template<typename T, typename U, int Dimensions>
inline std::common_type_t<T, U> dot(const Vec<T, Dimensions>& u, const Vec<U, Dimensions>& v)
{
    std::common_type_t<T, U> result{};
    for (int i = 0; i < Dimensions; ++i) {
        result += u[i] * v[i];
    }
    return result;
}

template<typename T, int Dimensions>
inline Vec<T, Dimensions> unit_vector(const Vec<T, Dimensions>& v)
{
    return v / v.length();
}
}
