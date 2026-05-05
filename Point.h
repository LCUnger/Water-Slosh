#pragma once

#include "Vec.h"

#include <array>
#include <concepts>
#include <cstddef>

namespace toolbox
{
template<typename T, std::size_t dimensions>
class Point
{
public:
    constexpr Point() = default;

    template<typename... Args>
        requires (sizeof...(Args) == dimensions && (std::convertible_to<Args, T> && ...))
    constexpr Point(Args... args) : elements_{ static_cast<T>(args)... } {}

    constexpr explicit Point(const std::array<T, dimensions>& values) : elements_(values) {}

    constexpr T at(std::size_t index) const { return elements_[index]; }

    constexpr const T& operator[](std::size_t index) const { return elements_[index]; }
    constexpr T& operator[](std::size_t index) { return elements_[index]; }

    constexpr Point& operator+=(const Vec<T, dimensions>& vector)
    {
        for (std::size_t index = 0; index < dimensions; ++index) {
            elements_[index] += vector[index];
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
}
