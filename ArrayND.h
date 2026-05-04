#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <algorithm>

template<typename T, std::size_t Rank>
class ArrayND
{
public:
    using value_type = T;
    using shape_type = std::array<std::size_t, Rank>;
    using index_type = std::array<std::size_t, Rank>;

    ArrayND() = default;

    explicit ArrayND(shape_type shape, std::size_t padding_width = 0)
        : shape_(shape),
          padding_width_(padding_width),
          elements_(totalSizeFromShape(shape, padding_width))
    {
        computeStrides();
    }

    ArrayND(shape_type shape, const T& initial_value, std::size_t padding_width = 0)
        : shape_(shape),
          padding_width_(padding_width),
          elements_(totalSizeFromShape(shape, padding_width), initial_value)
    {
        computeStrides();
    }

    T& operator[](std::size_t flat_index)
    {
        return elements_[flat_index];
    }

    const T& operator[](std::size_t flat_index) const
    {
        return elements_[flat_index];
    }

    T& at(const index_type& index)
    {
        return elements_.at(flattenIndex(index));
    }

    const T& at(const index_type& index) const
    {
        return elements_.at(flattenIndex(index));
    }

    template<typename... Indices>
        requires (sizeof...(Indices) == Rank)
    T& at(Indices... indices)
    {
        return at(index_type{ static_cast<std::size_t>(indices)... });
    }

    template<typename... Indices>
        requires (sizeof...(Indices) == Rank)
    const T& at(Indices... indices) const
    {
        return at(index_type{ static_cast<std::size_t>(indices)... });
    }

    template<typename... Indices>
        requires (sizeof...(Indices) == Rank)
    T& operator()(Indices... indices)
    {
        return (*this)[index_type{ static_cast<std::size_t>(indices)... }];
    }

    template<typename... Indices>
        requires (sizeof...(Indices) == Rank)
    const T& operator()(Indices... indices) const
    {
        return (*this)[index_type{ static_cast<std::size_t>(indices)... }];
    }

    const shape_type& shape() const
    {
        return shape_;
    }

    const shape_type& strides() const
    {
        return strides_;
    }

    std::size_t size() const
    {
        return elements_.size();
    }

    bool empty() const
    {
        return elements_.empty();
    }

    void fill(const T& value)
    {
        std::fill(elements_.begin(), elements_.end(), value);
    }

    ArrayND operator-() const
    {
        ArrayND result(shape_);

        for (std::size_t index = 0; index < size(); ++index) {
            result[index] = -elements_[index];
        }

        return result;
    }

    ArrayND& operator+=(const ArrayND& other)
    {
        assertSameShape(other);

        for (std::size_t index = 0; index < size(); ++index) {
            elements_[index] += other.elements_[index];
        }

        return *this;
    }

    ArrayND& operator-=(const ArrayND& other)
    {
        assertSameShape(other);

        for (std::size_t index = 0; index < size(); ++index) {
            elements_[index] -= other.elements_[index];
        }

        return *this;
    }

    template<typename U>
    ArrayND& operator*=(U scalar)
    {
        for (auto& element : elements_) {
            element *= static_cast<T>(scalar);
        }

        return *this;
    }

    template<typename U>
    ArrayND& operator/=(U scalar)
    {
        for (auto& element : elements_) {
            element /= static_cast<T>(scalar);
        }

        return *this;
    }

private:
    shape_type shape_{};
    shape_type strides_{};
    std::size_t padding_width_{};
    std::vector<T> elements_;

    void computeStrides()
    {
        std::size_t stride = 1;

        for (std::size_t axis = Rank; axis-- > 0;) {
            strides_[axis] = stride;
            stride *= shape_[axis] + 2 * padding_width_;
        }
    }

    static std::size_t totalSizeFromShape(const shape_type& shape, std::size_t padding_width)
    {
        std::size_t total = 1;

        for (std::size_t axis = 0; axis < Rank; ++axis) {
            total *= shape[axis] + 2 * padding_width;
        }

        return total;
    }

    std::size_t flattenIndex(const index_type& index) const
    {
        std::size_t flat_index = 0;

        for (std::size_t axis = 0; axis < Rank; ++axis) {
            if (index[axis] >= shape_[axis]) {
                throw std::out_of_range("ArrayND index out of bounds");
            }

            flat_index += (index[axis] + padding_width_) * strides_[axis];
        }

        return flat_index;
    }

    void assertSameShape(const ArrayND& other) const
    {
        if (shape_ != other.shape_) {
            throw std::invalid_argument("ArrayND shape mismatch");
        }
    }
};


template<typename T, typename U, std::size_t Rank>
ArrayND<std::common_type_t<T, U>, Rank>
operator+(const ArrayND<T, Rank>& left, const ArrayND<U, Rank>& right)
{
    if (left.shape() != right.shape()) {
        throw std::invalid_argument("ArrayND shape mismatch in operator+");
    }

    using ResultType = std::common_type_t<T, U>;

    ArrayND<ResultType, Rank> result(left.shape());

    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = left[index] + right[index];
    }

    return result;
}

template<typename T, typename U, std::size_t Rank>
ArrayND<std::common_type_t<T, U>, Rank>
operator-(const ArrayND<T, Rank>& left, const ArrayND<U, Rank>& right)
{
    if (left.shape() != right.shape()) {
        throw std::invalid_argument("ArrayND shape mismatch in operator-");
    }

    using ResultType = std::common_type_t<T, U>;

    ArrayND<ResultType, Rank> result(left.shape());

    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = left[index] - right[index];
    }

    return result;
}

template<typename T, typename U, std::size_t Rank>
ArrayND<std::common_type_t<T, U>, Rank>
operator*(const ArrayND<T, Rank>& array, U scalar)
{
    using ResultType = std::common_type_t<T, U>;

    ArrayND<ResultType, Rank> result(array.shape());

    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = array[index] * scalar;
    }

    return result;
}

template<typename T, typename U, std::size_t Rank>
ArrayND<std::common_type_t<T, U>, Rank>
operator*(U scalar, const ArrayND<T, Rank>& array)
{
    return array * scalar;
}

template<typename T, typename U, std::size_t Rank>
ArrayND<std::common_type_t<T, U>, Rank>
operator/(const ArrayND<T, Rank>& array, U scalar)
{
    using ResultType = std::common_type_t<T, U>;

    ArrayND<ResultType, Rank> result(array.shape());

    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = array[index] / scalar;
    }

    return result;
}