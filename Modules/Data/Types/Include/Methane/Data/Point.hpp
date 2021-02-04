/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License"),
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Data/Point.hpp
Point type wrapper around HLSL++ vector type

******************************************************************************/

#pragma once

#include "Vector.hpp"

#include <fmt/format.h>

#include <string>
#include <cstdint>

namespace Methane::Data
{

template<typename T, size_t size, typename = std::enable_if_t<std::is_arithmetic_v<T> && 2 <= size && size <= 4>>
class Point
{
public:
    using CoordinateType = T;
    using VectorType = HlslVector<T, size>;
    using PointType = Point<T, size>;

    static constexpr size_t dimensions_count = size;

    Point() = default;

    template<size_t sz = size, typename = std::enable_if_t<sz == 2, void>>
    Point(T x, T y) noexcept : m_vector(x, y) { }

    template<size_t sz = size, typename = std::enable_if_t<sz == 3, void>>
    Point(T x, T y, T z) noexcept : m_vector(x, y, z) { }

    template<size_t sz = size, typename = std::enable_if_t<sz == 4, void>>
    Point(T x, T y, T z, T w) noexcept : m_vector(x, y, z, w) { }

    template<typename ...TArgs, typename = std::enable_if_t<std::conjunction<std::is_arithmetic<TArgs>...>::value>>
    Point(TArgs... args) noexcept : m_vector(RoundCast(args)...) { } // NOSONAR - do not use explicit

    explicit Point(const std::array<T, size>& components) : m_vector(RawVector<T, size>(components).AsHlsl()) { }
    explicit Point(std::array<T, size>&& components) : m_vector(RawVector<T, size>(components).AsHlsl()) { }

    explicit Point(const VectorType& vector) noexcept : m_vector(vector) { }
    explicit Point(VectorType&& vector) noexcept : m_vector(std::move(vector)) { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 2>>
    explicit Point(const HlslVector<V, 2>& vector) noexcept
        : m_vector(RoundCast(vector.x), RoundCast(vector.y))
    { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 3>>
    explicit Point(const HlslVector<V, 3>& vector) noexcept
        : m_vector(RoundCast(vector.x), RoundCast(vector.y), RoundCast(vector.z))
    { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 4>>
    explicit Point(const HlslVector<V, 4>& vector) noexcept
        : m_vector(RoundCast(vector.x), RoundCast(vector.y), RoundCast(vector.z), RoundCast(vector.w))
    { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 2>>
    explicit Point(const Point<V, 2>& other) noexcept
        : m_vector(RoundCast(other.GetX()), RoundCast(other.GetY()))
    { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 3>>
    explicit Point(const Point<V, 3>& other) noexcept
        : m_vector(RoundCast(other.GetX()), RoundCast(other.GetY()), RoundCast(other.GetZ()))
    { }

    template<typename V, size_t sz = size, typename = std::enable_if_t<sz == 4>>
    explicit Point(const Point<V, 4>& other) noexcept
        : m_vector(RoundCast(other.GetX()), RoundCast(other.GetY()), RoundCast(other.GetZ()), RoundCast(other.GetW()))
    { }

    VectorType& AsVector() noexcept               { return m_vector; }
    const VectorType& AsVector() const noexcept   { return m_vector; }

    T GetX() const noexcept { return m_vector.x; }
    T GetY() const noexcept { return m_vector.y; }

    template<size_t sz = size, typename = std::enable_if_t<sz >= 3, void>>
    T GetZ() const noexcept { return m_vector.z; }

    template<size_t sz = size, typename = std::enable_if_t<sz >= 4, void>>
    T GetW() const noexcept { return m_vector.w; }

    PointType& SetX(T x) noexcept { m_vector.x = x; return *this; }
    PointType& SetY(T y) noexcept { m_vector.y = y; return *this; }

    template<size_t sz = size>
    std::enable_if_t<sz >= 3, PointType&> SetZ(T z) noexcept { m_vector.z = z; return *this; }

    template<size_t sz = size>
    std::enable_if_t<sz >= 4, PointType&> SetW(T w) noexcept { m_vector.w = w; return *this; }

    T GetLength() const noexcept { return static_cast<T>(hlslpp::length(m_vector)); }
    T GetLengthSquared() const noexcept
    {
        T sq_length = Square(m_vector.x);
        sq_length += Square(m_vector.y);
        if constexpr (size > 2)
            sq_length += Square(m_vector.z);
        if constexpr (size > 3)
            sq_length += Square(m_vector.w);
        return sq_length;
    }

    PointType& Normalize() noexcept
    {
        if constexpr (std::is_same_v<float, T>)
            m_vector = hlslpp::normalize(m_vector);
        else
            m_vector /= GetLength();
        return *this;
    }

    bool operator==(const PointType& other) const noexcept { return hlslpp::all(m_vector == other.AsVector()); }
    bool operator!=(const PointType& other) const noexcept { return !operator==(other); }
    bool operator<(const PointType& other) const noexcept  { return hlslpp::all(m_vector <  other.AsVector()); }
    bool operator<=(const PointType& other) const noexcept { return hlslpp::all(m_vector <= other.AsVector()); }
    bool operator>(const PointType& other) const noexcept  { return hlslpp::all(m_vector >  other.AsVector()); }
    bool operator>=(const PointType& other) const noexcept { return hlslpp::all(m_vector >= other.AsVector()); }

    PointType operator+(const PointType& other) const noexcept { return PointType(m_vector + other.AsVector()); }
    PointType operator-(const PointType& other) const noexcept { return PointType(m_vector - other.AsVector()); }
    PointType& operator+=(const PointType& other) noexcept     { m_vector += other.AsVector(); return *this; }
    PointType& operator-=(const PointType& other) noexcept     { m_vector -= other.AsVector(); return *this; }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType> operator*(M multiplier) const noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            return PointType(m_vector * multiplier);
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                return PointType(Point<M, size>(*this) * multiplier);
            else
                return PointType(m_vector * static_cast<T>(multiplier));
        }
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType> operator/(M divisor) const noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            return PointType(m_vector / divisor);
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                return PointType(Point<M, size>(*this) / divisor);
            else
                return PointType(m_vector / static_cast<T>(divisor));
        }
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType> operator*(const Point<M, size>& multiplier) const noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            return PointType(m_vector * multiplier.AsVector());
        else
        {
            if constexpr (std::is_floating_point_v<M>)
            {
                return PointType(Point<M, size>(static_cast<Point<M, size>>(*this).AsVector() * multiplier.AsVector()));
            }
            else
                return PointType(m_vector * static_cast<PointType>(multiplier).AsVector());
        }
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType> operator/(const Point<M, size>& divisor) const noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            return PointType(m_vector / divisor.AsVector());
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                return PointType(Point<M, size>(static_cast<Point<M, size>>(*this).AsVector() / divisor.AsVector()));
            else
                return PointType(m_vector / static_cast<PointType>(divisor).AsVector());
        }
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType&> operator*=(M multiplier) noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            m_vector *= multiplier;
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                *this = PointType(Point<M, size>(*this) * multiplier);
            else
                m_vector *= static_cast<T>(multiplier);
        }
        return *this;
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType&> operator/=(M divisor) noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            m_vector /= divisor;
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                *this = PointType(Point<M, size>(*this) / divisor);
            else
                m_vector /= static_cast<T>(divisor);
        }
        return *this;
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType&> operator*=(const Point<M, size>& multiplier) noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            m_vector *= multiplier.AsVector();
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                *this = PointType(Point<M, size>(*this) * multiplier);
            else
                m_vector *= static_cast<PointType>(multiplier).AsVector();
        }
        return *this;
    }

    template<typename M>
    std::enable_if_t<std::is_arithmetic_v<M>, PointType&> operator/=(const Point<M, size>& divisor) noexcept
    {
        if constexpr (std::is_same_v<T, M>)
            m_vector /= divisor.AsVector();
        else
        {
            if constexpr (std::is_floating_point_v<M>)
                *this = PointType(Point<M, size>(*this) / divisor);
            else
                m_vector /= static_cast<PointType>(divisor).AsVector();
        }
        return *this;
    }

    template<typename U, typename = std::enable_if_t<!std::is_same_v<T, U>>>
    explicit operator Point<U, size>() const noexcept
    {
        if constexpr (size == 2)
            return Point<U, 2>(RoundCast<U>(GetX()), RoundCast<U>(GetY()));
        else if constexpr (size == 3)
            return Point<U, 3>(RoundCast<U>(GetX()), RoundCast<U>(GetY()), RoundCast<U>(GetZ()));
        else if constexpr (size == 4)
            return Point<U, 4>(RoundCast<U>(GetX()), RoundCast<U>(GetY()), RoundCast<U>(GetZ()), RoundCast<U>(GetW()));
    }

    explicit operator std::string() const
    {
        if constexpr (size == 2)
            return fmt::format("P({}, {})", GetX(), GetY());
        else if constexpr (size == 3)
            return fmt::format("P({}, {}, {})", GetX(), GetY(), GetZ());
        else if constexpr (size == 4)
            return fmt::format("P({}, {}, {}, {})", GetX(), GetY(), GetZ(), GetW());
    }

    explicit operator VectorType() const noexcept { return m_vector; }

    const VectorType& AsHlsl() const noexcept { return m_vector; }

private:
    static inline T Square(T s) noexcept { return s * s; }

    template<typename V>
    constexpr T RoundCast(V value) noexcept
    {
        if constexpr (std::is_same_v<T, V>)
            return value;
        else
        {
            if constexpr (std::is_integral_v<T> && std::is_floating_point_v<V>)
                return static_cast<T>(std::round(value));
            else
                return static_cast<T>(value);
        }
    }

    VectorType m_vector;
};

template<typename T>
using Point2T = Point<T, 2>;

using Point2I = Point2T<int32_t>;
using Point2U = Point2T<uint32_t>;
using Point2F = Point2T<float>;
using Point2D = Point2T<double>;

} // namespace Methane::Data
