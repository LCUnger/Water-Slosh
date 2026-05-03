#pragma once

#include "Vec.h"

namespace toolbox
{
template<typename T, int Dimensions>
class Point
{
public:
    constexpr Point() = default;

    template<typename... Args>
        requires (sizeof...(Args) == Dimensions && (std::convertible_to<Args, T> && ...))
    constexpr Point(Args... args) : e{ static_cast<T>(args)... } {}

    constexpr explicit Point(const std::array<T, Dimensions>& values) : e(values) {}

    constexpr T at(int i) const { return e[i]; }

    constexpr const T& operator[](int i) const { return e[i]; }
    constexpr T& operator[](int i) { return e[i]; }

    constexpr Point& operator+=(const Vec<T, Dimensions>& v)
    {
        for (int i = 0; i < Dimensions; ++i) {
            e[i] += v[i];
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
}