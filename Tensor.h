#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

template<typename T, std::size_t Rank, std::size_t padding_width, std::size_t... Extents>
class Tensor
{
    static_assert(Rank == sizeof...(Extents));
    static constexpr std::array<std::size_t, Rank> extents{ Extents... };
    static constexpr std::size_t size = ((Extents + 2 * padding_width) * ...);

public:
    constexpr Tensor() = default;

    template<typename... Args>
        requires (sizeof...(Args) == size && (std::convertible_to<Args, T> && ...))
    constexpr Tensor(Args... args) : elements_{ static_cast<T>(args)... } {}

    constexpr Tensor operator-() const
    {
        Tensor result;
        for (std::size_t index = 0; index < size; ++index) {
            result[index] = -elements_[index];
        }
        return result;
    }

    constexpr Tensor& operator+=(const Tensor& other)
    {
        for (std::size_t index = 0; index < size; ++index) {
            elements_[index] += other.elements_[index];
        }
        return *this;
    }

    constexpr Tensor& operator-=(const Tensor& other)
    {
        for (std::size_t index = 0; index < size; ++index) {
            elements_[index] -= other.elements_[index];
        }
        return *this;
    }

    template<typename U>
    constexpr Tensor& operator*=(const U scalar)
    {
        for (auto& component : elements_) {
            component *= static_cast<T>(scalar);
        }
        return *this;
    }

    template<typename U>
    constexpr Tensor& operator/=(const U scalar)
    {
        for (auto& component : elements_) {
            component /= static_cast<T>(scalar);
        }
        return *this;
    }

    constexpr T& operator[](std::size_t index) { return elements_[index]; }
    constexpr const T& operator[](std::size_t index) const { return elements_[index]; }

    template<typename... Indices>
    constexpr T& at(Indices... indices)
        requires (sizeof...(Indices) == Rank)
    {
        return elements_[flattenIndices(static_cast<std::size_t>(indices) + padding_width ...)];
    }

    template<typename... Indices>
    constexpr const T& at(Indices... indices) const
        requires (sizeof...(Indices) == Rank)
    {
        return elements_[flattenIndices(static_cast<std::size_t>(indices) + padding_width ...)];
    }

private:
    std::array<T, size> elements_{};

    template<typename... Indices>
    static constexpr std::size_t flattenIndices(Indices... indices)
    {
        std::array<std::size_t, Rank> idx{ static_cast<std::size_t>(indices)... };
        std::size_t offset = 0;
        std::size_t stride = 1;

        for (std::size_t i = Rank; i-- > 0;) {
            offset += idx[i] * stride;
            stride *= extents[i] + 2 * padding_width;
        }

        return offset;
    }
};

template<typename T, typename U, std::size_t Rank, std::size_t ghost_width, std::size_t... Extents>
inline Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> operator+(const Tensor<T, Rank, ghost_width, Extents...>& left, const Tensor<U, Rank, ghost_width, Extents...>& right)
{
    constexpr std::size_t tensor_size = ((Extents + 2 * ghost_width) * ...);
    Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> result;
    for (std::size_t index = 0; index < tensor_size; ++index) {
        result[index] = left[index] + right[index];
    }
    return result;
}

template<typename T, typename U, std::size_t Rank, std::size_t ghost_width, std::size_t... Extents>
inline Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> operator-(const Tensor<T, Rank, ghost_width, Extents...>& left, const Tensor<U, Rank, ghost_width, Extents...>& right)
{
    constexpr std::size_t tensor_size = ((Extents + 2 * ghost_width) * ...);
    Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> result;
    for (std::size_t index = 0; index < tensor_size; ++index) {
        result[index] = left[index] - right[index];
    }
    return result;
}

template<typename T, typename U, std::size_t Rank, std::size_t ghost_width, std::size_t... Extents>
inline Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> operator*(const Tensor<T, Rank, ghost_width, Extents...>& tensor, U scalar)
{
    constexpr std::size_t tensor_size = ((Extents + 2 * ghost_width) * ...);
    Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> result;
    for (std::size_t index = 0; index < tensor_size; ++index) {
        result[index] = tensor[index] * scalar;
    }
    return result;
}

template<typename T, typename U, std::size_t Rank, std::size_t ghost_width, std::size_t... Extents>
inline Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> operator*(U scalar, const Tensor<T, Rank, ghost_width, Extents...>& tensor)
{
    return tensor * scalar;
}

template<typename T, typename U, std::size_t Rank, std::size_t ghost_width, std::size_t... Extents>
inline Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> operator/(const Tensor<T, Rank, ghost_width, Extents...>& tensor, U scalar)
{
    constexpr std::size_t tensor_size = ((Extents + 2 * ghost_width) * ...);
    Tensor<std::common_type_t<T, U>, Rank, ghost_width, Extents...> result;
    for (std::size_t index = 0; index < tensor_size; ++index) {
        result[index] = tensor[index] / scalar;
    }
    return result;
}