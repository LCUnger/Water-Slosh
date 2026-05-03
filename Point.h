#pragma once

#include "Vec.h"

namespace toolbox
{
template<typename T, int dimensions>
class Point
{
public:
    constexpr Point() = default;

    template<typename... Args>
        requires (sizeof...(Args) == dimensions && (std::convertible_to<Args, T> && ...))
    constexpr Point(Args... args) : elements{ static_cast<T>(args)... } {}

    constexpr explicit Point(const std::array<T, dimensions>& values) : elements(values) {}

    constexpr T at(int index) const { return elements[index]; }

    constexpr const T& operator[](int index) const { return elements[index]; }
    constexpr T& operator[](int index) { return elements[index]; }

    constexpr Point& operator+=(const Vec<T, dimensions>& vector)
    {
        for (int index = 0; index < dimensions; ++index) {
            elements[index] += vector[index];
        }
        return *this;
    }

    constexpr auto begin() { return elements.begin(); }
    constexpr auto end() { return elements.end(); }
    constexpr auto begin() const { return elements.begin(); }
    constexpr auto end() const { return elements.end(); }

    constexpr auto cbegin() const { return elements.cbegin(); }
    constexpr auto cend() const { return elements.cend(); }

private:
    std::array<T, dimensions> elements{};
};
}