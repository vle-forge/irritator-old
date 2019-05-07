// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
#define ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP

#include <irritator/data-array.hpp>
#include <irritator/export.hpp>
#include <irritator/string.hpp>

#include <filesystem>
#include <memory>
#include <optional>

namespace irr {

constexpr int max_name_length = 8;

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

struct NamedValue
{
    enum class named_value_type : int8_t
    {
        boolean,   // bool
        integer32, // int32_t
        integer64, // int64_t
        real32,    // float
        real64,    // double
        string,    // std::string
        // set,       // std::vector<named_value>
        // map        // std::map<string, named_value>
    };

    string<max_name_length> name;
    ListID named_values;
    named_value_type type = named_value_type::boolean;
};

struct Condition
{
    string<max_name_length> name;
    ListID named_values = -1;
};

struct GuiNode
{
    float x = 0.f;
    float y = 0.f;
    float height = 0.f;
    float width = 0.f;
    bool selected = false;
};

struct Connection
{
    ID input_model = 0;
    ID output_model = 0;
    ID input_slot = 0;
    ID output_slot = 0;
};

struct GuiSlot
{
    vec2 position;
};

struct Slot
{
    string<8> name;
};

struct GuiConnection
{
    vec2 point[3];
};

struct Dynamic
{
    ID name;

    string<128> package_name;
    string<128> library_name;
};

struct View
{
    enum view_option : std::int8_t
    {
        timed = 0,
        alloc = 1 << 1,
        output = 1 << 2,
        internal = 1 << 3,
        external = 1 << 4,
        confluent = 1 << 5,
        finish = 1 << 6,
    };

    enum class view_type : std::int8_t
    {
        csv,
        json
    };

    float time_step = 1.f;
    string<1024> output_path;
    view_option options = view_option::timed;
    view_type type = view_type::json;
};

struct Node
{
    enum class model_type : std::int8_t
    {
        atomic,
        coupled
    };

    ID parent = 0;
    string<8> name;
    int input_slots_number = 0;
    int output_slots_number = 0;

    ID dynamics = 0;
    ListID conditions = -1;
    ListID observables = -1;

    ListID children = -1;
    ListID connections = -1;

    model_type type = model_type::atomic;
};

struct Class
{
    ID name;
    ID model;
};

// Simulation part

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

struct Context
{
    enum class message_type
    {
        emerg = 0, ///< system is unusable
        alert,     ///< action must be taken immediately
        crit,      ///< critical conditions
        err,       ///< error conditions
        warning,   ///< warning conditions
        notice,    ///< normal, but significant, condition
        info,      ///< informational message
        debug,     ///< debug-level message
    };

    Context() = default;

    bool init(int verbose_level = 6) noexcept;
    bool init(std::FILE* f, int verbose_level = 6) noexcept;

    std::FILE* cfile_logger = stdout;
    message_type log_priority = Context::message_type::info;
};

using NamedValues = data_array<NamedValue, ID>;
using Conditions = data_array<Condition, ID>;

using Nodes = data_array<Node, ID>;
using Connections = data_array<Connection, ID>;
using Slots = data_array<Slot, ID>;
using Views = data_array<View, ID>;
using Dynamics = data_array<Dynamic, ID>;
using Classes = data_array<Class, ID>;

struct Model
{
    Model(int estimated_model_number = 4096);

    void read(Context& context, const std::filesystem::path& file_name);

    string<32> name;
    string<32> author;
    int version_major;
    int version_minor;
    int version_patch;

    NamedValues named_values;
    Conditions conditions;
    Connections connections;
    Slots slots;
    Views views;
    Nodes nodes;
    Classes classes;
};

struct VLE
{
    VLE();

    std::array<std::filesystem::path, 4> paths;
    Context context;
    Dynamics dynamics;
};

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
