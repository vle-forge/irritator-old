// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
#define ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP

#include <irritator/data-array.hpp>
#include <irritator/export.hpp>
#include <irritator/string.hpp>

namespace irr {

struct vec2
{
    float x, y;

    constexpr vec2() noexcept
      : x(0.f)
      , y(0.f)
    {}

    constexpr vec2(float x_, float y_) noexcept
      : x(x_)
      , y(y_)
    {}

    friend constexpr vec2 operator*(const vec2& lhs, const float rhs) noexcept
    {
        return vec2(lhs.x * rhs, lhs.y * rhs);
    }

    friend constexpr vec2 operator/(const vec2& lhs, const float rhs) noexcept
    {
        return vec2(lhs.x / rhs, lhs.y / rhs);
    }

    friend constexpr vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    friend constexpr vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    friend constexpr vec2 operator*(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
    }

    friend constexpr vec2 operator/(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
    }

    friend constexpr vec2& operator+=(vec2& lhs, const vec2& rhs) noexcept
    {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    friend constexpr vec2& operator-=(vec2& lhs, const vec2& rhs) noexcept
    {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }

    friend constexpr vec2& operator*=(vec2& lhs, const float rhs) noexcept
    {
        lhs.x *= rhs;
        lhs.y *= rhs;
        return lhs;
    }

    friend constexpr vec2& operator/=(vec2& lhs, const float rhs) noexcept
    {
        lhs.x /= rhs;
        lhs.y /= rhs;
        return lhs;
    }
};

struct vec3
{
    float x, y, z;

    constexpr vec3() noexcept
      : x(0.f)
      , y(0.f)
      , z(0.f)
    {}

    constexpr vec3(float x_, float y_, float z_) noexcept
      : x(x_)
      , y(y_)
      , z(z_)
    {}

    friend constexpr vec3 operator*(const vec3& lhs, const float rhs) noexcept
    {
        return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    }

    friend constexpr vec3 operator/(const vec3& lhs, const float rhs) noexcept
    {
        return vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
    }

    friend constexpr vec3 operator+(const vec3& lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    friend constexpr vec3 operator-(const vec3& lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    friend constexpr vec3 operator*(const vec3& lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * lhs.z);
    }

    friend constexpr vec3 operator/(const vec3& lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    friend constexpr vec3& operator+=(vec3& lhs, const vec3& rhs) noexcept
    {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }

    friend constexpr vec3& operator-=(vec3& lhs, const vec3& rhs) noexcept
    {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }

    friend constexpr vec3& operator*=(vec3& lhs, const float rhs) noexcept
    {
        lhs.x *= rhs;
        lhs.y *= rhs;
        lhs.z *= rhs;
        return lhs;
    }

    friend constexpr vec3& operator/=(vec3& lhs, const float rhs) noexcept
    {
        lhs.x /= rhs;
        lhs.y /= rhs;
        lhs.z /= rhs;
        return lhs;
    }
};

struct Value
{
    enum class value_type : int8_t
    {
        none,      // empty message
        integer32, // int32_t
        integer64, // int64_t
        real32,    // float
        real64,    // double
        vec2_32,   // float[2]
        vec3_32    // float[3]
    };

    int32_t index;
    int16_t size;
    value_type type = value_type::none;
};

struct Values
{
    array<int32_t> integer32;
    array<int64_t> integer64;
    array<float> real32;
    array<double> real64;
    array<vec2> vec2_32;
    array<vec3> vec3_32;

    int next_integer32 = 0;
    int next_integer64 = 0;
    int next_real32 = 0;
    int next_real64 = 0;
    int next_vec2_32 = 0;
    int next_vec3_32 = 0;

    Values(int capacity)
      : integer32(capacity)
      , integer64(capacity)
      , real32(capacity)
      , real64(capacity)
      , vec2_32(capacity)
      , vec3_32(capacity)
    {
        assert(capacity > 0);
    }

    Value alloc_integer32(int32_t value) noexcept
    {
        const auto next = next_integer32++;
        assert(next < INT32_MAX);

        integer32[next] = value;
        return Value{ next, 1, Value::value_type::integer32 };
    }

    Value alloc_integer64(int64_t value) noexcept
    {
        const auto next = next_integer64++;
        assert(next < INT32_MAX);

        integer64[next] = value;
        return Value{ next, 1, Value::value_type::integer64 };
    }

    Value alloc_real32(float value) noexcept
    {
        const auto next = next_real32++;
        assert(next < INT32_MAX);

        real32[next] = value;
        return Value{ next, 1, Value::value_type::real32 };
    }

    Value alloc_real64(double value) noexcept
    {
        const auto next = next_real64++;
        assert(next < INT32_MAX);

        real64[next] = value;
        return Value{ next, 1, Value::value_type::real64 };
    }

    Value alloc_vec2_32(vec2 value) noexcept
    {
        const auto next = next_vec2_32++;
        assert(next < INT32_MAX);

        vec2_32[next] = value;
        return Value{ next, 1, Value::value_type::vec2_32 };
    }

    Value alloc_vec3_32(vec3 value) noexcept
    {
        const auto next = next_vec3_32++;
        assert(next < INT32_MAX);

        vec3_32[next] = value;
        return Value{ next, 1, Value::value_type::vec3_32 };
    }

    Value alloc_integer32(int32_t value, int16_t length) noexcept
    {
        const auto next = next_integer32;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_integer32 += length;
        integer32[next] = value;
        return Value{ next, length, Value::value_type::integer32 };
    }

    Value alloc_integer64(int64_t value, int16_t length) noexcept
    {
        const auto next = next_integer64;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_integer64 += length;
        integer64[next] = value;
        return Value{ next, length, Value::value_type::integer64 };
    }

    Value alloc_real32(float value, int16_t length) noexcept
    {
        const auto next = next_real32;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_real32 += length;
        real32[next] = value;
        return Value{ next, length, Value::value_type::real32 };
    }

    Value alloc_real64(double value, int16_t length) noexcept
    {
        const auto next = next_real64;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_real64 += length;
        real64[next] = value;
        return Value{ next, length, Value::value_type::real64 };
    }

    Value alloc_vec2_32(vec2 value, int16_t length) noexcept
    {
        const auto next = next_vec2_32;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_vec2_32 += length;
        vec2_32[next] = value;
        return Value{ next, length, Value::value_type::vec2_32 };
    }

    Value alloc_vec3_32(vec3 value, int16_t length) noexcept
    {
        const auto next = next_vec3_32;
        assert(length > 0);
        assert(INT32_MAX - next > length);

        next_vec3_32 += length;
        vec3_32[next] = value;
        return Value{ next, length, Value::value_type::vec3_32 };
    }

    void clear()
    {
        next_integer32 = 0;
        next_integer64 = 0;
        next_real32 = 0;
        next_real64 = 0;
        next_vec2_32 = 0;
        next_vec3_32 = 0;
    }

    template<typename T>
    auto* get(const Value& v) noexcept
    {
        switch (v.type) {
        case Value::value_type::integer32:
            return &integer32[v.index];
        case Value::value_type::integer64:
            return &integer64[v.index];
        case Value::value_type::real32:
            return &real32[v.index];
        case Value::value_type::real64:
            return &real64[v.index];
        case Value::value_type::vec2_32:
            return &vec2_32[v.index];
        case Value::value_type::vec3_32:
            return &vec3_32[v.index];
        default:
            return nullptr;
        }
    }
};

struct Simulator
{
    ID dynamics;

    int input_slots_number;
    int output_slots_number;

    float tl;
    float tn;
};

struct FlatSimulation
{
    ID model;

    float start;
    float current;
    float end;
};

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
