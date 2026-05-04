#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>

namespace toolbox
{
template<typename T, int dimensions>
class Vec
{
public:
    constexpr Vec() = default;

    template<typename... Args>
        requires (sizeof...(Args) == dimensions && (std::convertible_to<Args, T> && ...))
    constexpr Vec(Args... args) : elements_{ static_cast<T>(args)... } {}

    constexpr explicit Vec(const std::array<T, dimensions>& values) : elements_(values) {}

    constexpr T at(int index) const { return elements_[index]; }

    constexpr const T& operator[](int index) const { return elements_[index]; }
    constexpr T& operator[](int index) { return elements_[index]; }

    constexpr Vec operator-() const
    {
        Vec result;
        for (int index = 0; index < dimensions; ++index) {
            result[index] = -elements_[index];
        }
        return result;
    }

    constexpr Vec& operator+=(const Vec& vector)
    {
        for (int index = 0; index < dimensions; ++index) {
            elements_[index] += vector.elements_[index];
        }
        return *this;
    }

    constexpr Vec& operator-=(const Vec& vector)
    {
        for (int index = 0; index < dimensions; ++index) {
            elements_[index] -= vector.elements_[index];
        }
        return *this;
    }

    template<typename U>
    constexpr Vec& operator*=(const U scalar)
    {
        for (auto& component : elements_) {
            component *= static_cast<T>(scalar);
        }
        return *this;
    }

    template<typename U>
    constexpr Vec& operator/=(const U scalar)
    {
        for (auto& component : elements_) {
            component /= static_cast<T>(scalar);
        }
        return *this;
    }

    T length_squared() const requires std::floating_point<T>
    {
        T sum{};
        for (const auto& component : elements_) {
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
        for (const auto& component : elements_) {
            if (std::fabs(component) >= s) {
                return false;
            }
        }
        return true;
    }

	Vec& fill(const T value)
    {
        for (auto& component : elements_) {
            component = value;
        }
        return *this;
    }

    constexpr auto begin() { return elements_.begin(); }
    constexpr auto end() { return elements_.end(); }
    constexpr auto begin() const { return elements_.begin(); }
    constexpr auto end() const { return elements_.end(); }

    constexpr auto cbegin() const { return elements_.cbegin(); }
    constexpr auto cend() const { return elements_.cend(); }

private:
    std::array<T, dimensions> elements_{};
};

template<typename T, int dimensions>
inline std::ostream& operator<<(std::ostream& out, const Vec<T, dimensions>& vector)
{
    for (int index = 0; index < dimensions; ++index) {
        if (index > 0) {
            out << ' ';
        }
        out << vector[index];
    }
    return out;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator+(const Vec<T, dimensions>& left, const Vec<U, dimensions>& right)
{
    Vec<std::common_type_t<T, U>, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = left[index] + right[index];
    }
    return result;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator-(const Vec<T, dimensions>& left, const Vec<U, dimensions>& right)
{
    Vec<std::common_type_t<T, U>, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = left[index] - right[index];
    }
    return result;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator*(const Vec<T, dimensions>& left, const Vec<U, dimensions>& right)
{
    Vec<std::common_type_t<T, U>, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = left[index] * right[index];
    }
    return result;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator*(const Vec<T, dimensions>& vector, U scalar)
{
    Vec<std::common_type_t<T, U>, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = vector[index] * scalar;
    }
    return result;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator*(U scalar, const Vec<T, dimensions>& vector)
{
    return vector * scalar;
}

template<typename T, typename U, int dimensions>
inline Vec<std::common_type_t<T, U>, dimensions> operator/(const Vec<T, dimensions>& vector, U scalar)
{
    Vec<std::common_type_t<T, U>, dimensions> result;
    for (int index = 0; index < dimensions; ++index) {
        result[index] = vector[index] / scalar;
    }
    return result;
}

template<typename T, typename U, int dimensions>
inline std::common_type_t<T, U> dot(const Vec<T, dimensions>& left, const Vec<U, dimensions>& right)
{
    std::common_type_t<T, U> result{};
    for (int index = 0; index < dimensions; ++index) {
        result += left[index] * right[index];
    }
    return result;
}

template<typename T, int dimensions>
inline Vec<T, dimensions> unit_vector(const Vec<T, dimensions>& vector)
{
    return vector / vector.length();
}
}
