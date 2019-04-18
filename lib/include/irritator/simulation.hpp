// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
#define ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP

#include <irritator/data-array.hpp>
#include <irritator/export.hpp>

#include <filesystem>
#include <memory>
#include <optional>

namespace irr {

constexpr unsigned max_name_length = 8;
constexpr unsigned max_slot_name_length = 5;
constexpr unsigned max_buffer_length = 1024;

struct Name
{
    char string[max_name_length];
};

struct Description
{
    char buffer[max_buffer_length];
};

struct SlotName
{
    char string[max_slot_name_length];
};

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

enum class value_type
{
    none,
    integer32,
    integer64,
    real32,
    real64,
    vec2_32,
    vec3_32
};

struct Value
{
    ID value;
    value_type type = value_type::none;
};

struct NamedValue
{
    ID name;
    ID value;
    value_type type = value_type::none;
};

struct Condition
{
    ID name;
    ListID values;
};

struct Node
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
    ID input_slot = 0;
    ID output_model = 0;
    ID output_slot = 0;
};

struct Slot
{
    ID name = 0;
    ListID values = -1;
};

struct CoupledModel
{
    ListID children = -1;
    ListID connections = -1;
};

enum class dynamics_type
{
    interpreted,
    internal,
    shared_library
};

enum class internal_dynamics_type
{
    counter,
    generator,
    observer,
};

class CodeDynamic
{
public:
    virtual void init(int capacity) = 0;
    virtual ID alloc() = 0;
    virtual void free(ID model) = 0;
    virtual int size() const noexcept = 0;
    virtual double start(ID model, double time) = 0;
    virtual double transition(ID model, double time) = 0;
    virtual void output(ID model) = 0;
};

struct InterpretedDynamic
{};

struct InternalDynamic
{
    ID id;
    internal_dynamics_type type;

    CodeDynamic* dynamics = nullptr;
};

struct SharedLibraryDynamic
{
    ID package;
    ID library;

    CodeDynamic* dynamics = nullptr;
};

struct Dynamic
{
    Dynamic() noexcept = default;

    ID name;

    union type
    {
        InterpretedDynamic interpreted;
        InternalDynamic internal;
        SharedLibraryDynamic shared_library;
    };
};

enum view_option
{
    timed = 0,
    alloc = 1 << 1,
    output = 1 << 2,
    internal = 1 << 3,
    external = 1 << 4,
    confluent = 1 << 5,
    finish = 1 << 6,
};

enum class view_type
{
    csv,
    json
};

struct View
{
    float time_step = 1.f;

    view_option options = view_option::timed;
    view_type type = view_type::json;
};

struct AtomicModel
{
    ID dynamics = 0;
    ListID conditions = -1;
    ListID observables = -1;
};

enum class model_type
{
    atomic,
    coupled
};

struct BaseModel
{
    BaseModel() noexcept = default;

    ID parent = 0;
    ID name = 0;
    ID model = 0;
    ListID input_slots = -1;
    ListID output_slots = -1;

    union type
    {
        AtomicModel atomic;
        CoupledModel coupled;
    };

    model_type type = model_type::atomic;
};

struct Class
{
    ID name;
    ID model;
};

struct Simulator
{
    ID atomic_model;

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

using Names = data_array<Name, ID>;
using Descriptions = data_array<Description, ID>;
using SlotNames = data_array<SlotName, ID>;

using Int32s = data_array<std::int32_t, ID>;
using Int64s = data_array<std::int64_t, ID>;
using Real32s = data_array<float, ID>;
using Real64s = data_array<double, ID>;
using Vec2_32s = data_array<vec2, ID>;
using Vec3_32s = data_array<vec3, ID>;

using Values = data_array<Value, WID>;
using NamedValues = data_array<NamedValue, WID>;

using Conditions = data_array<Condition, ID>;

using Nodes = data_array<Node, ID>;
using Connections = data_array<Connection, ID>;
using Slots = data_array<Slot, ID>;
using Views = data_array<View, ID>;
using Dynamics = data_array<Dynamic, ID>;
using BaseModels = data_array<BaseModel, ID>;
using Classes = data_array<Class, ID>;

using Simulators = data_array<Simulator, ID>;

inline void
string_copy(Name& name, const char* str, unsigned length)
{
    unsigned max = std::max(max_name_length - 1, length);
    strncpy(name.string, str, max);
    name.string[max] = '\0';
}

inline bool
is_string_equal(Name& name, const char* str)
{
    return strncmp(name.string, str, max_name_length) == 0;
}

struct Model
{
    Model(int estimated_model_number = 4096);

    void read(Context& context, const std::filesystem::path& file_name);

    ID name;
    ID author;
    int version_major;
    int version_minor;
    int version_patch;

    Names names;
    Descriptions descriptions;
    SlotNames slot_names;
    Int32s int32s;
    Int64s int64s;
    Real32s real32s;
    Real64s real64s;
    Vec2_32s vec2_32s;
    Vec3_32s vec3_32s;
    Values values;
    NamedValues named_values;
    Conditions conditions;
    Nodes nodes;
    Connections connections;
    Slots slots;
    Views views;
    BaseModels base_models;
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
