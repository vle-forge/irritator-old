// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
#define ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP

#include <irritator/data-array.hpp>
#include <irritator/linker.hpp>

#include <array>
#include <string>

namespace irr {

struct ID2
{
    constexpr ID2() noexcept = default;

    constexpr ID2(ID id) noexcept
      : id1(id)
      , id2(id)
    {}

    constexpr ID2(ID id1_, ID id2_) noexcept
      : id1(id1_)
      , id2(id2_)
    {}

    ID id1{ 0 };
    ID id2{ 0 };
};

/**
 * @brief Store atomic and executive behaviours.
 * @details package and library specifies the package where found the library,
 *     library is the shared library to pickup the dynamics.
 *
 */
struct Dynamics
{
    std::string name;
    std::string package;   // where to found the shared library.
    std::string library;   // shared library name (toto equals libtoto.so or
                           // libtoto.dylib or libtoto.dll).
    void* alloc = nullptr; // pointer to the factory.

    enum class dynamics_type
    {
        local,
        cpp,
        js
    };

    dynamics_type type = dynamics_type::local;
};

struct View
{
    std::string name;
    std::string location;
    double timestep = 1.0;

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

    enum class output_type
    {
        csv,
        json
    };

    unsigned view = view_option::timed;
    output_type type = output_type::csv;
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
    value_type type = value_type::none;
};

struct NamedValue
{
    std::string name;
    value_type type = value_type::none;
};

struct Condition
{
    std::string name;
};

struct Port
{
    std::string name;

    /* plus list of input or output message */
};

struct Model
{
    std::string name;

    enum class model_type
    {
        atomic,
        coupled
    };

    std::int16_t input_slot_number = 0;
    std::int16_t output_slot_number = 0;
    model_type type = model_type::atomic;
};

// struct GuiModel
// {
//     ImVec2 pos = { 0, 0 };
//     ImVec2 size = { 0, 0 };
//     bool selected = { false };
// };

// struct GuiPort
// {
//     ImVec2 pos = { 0, 0 };
//     ImVec2 size = { 0, 0 };
//     bool selected = { false };
// };

struct Class
{
    std::string name;
};

struct SimDynamics
{
    // 1) classical hierarchy model? => no
    // 2) function with state:
    // std::function<Time(State, Time)> start;
    // std::function<Time(State, Time)> transition;
    // std::function<void(State)> output;
    // 3) Each library gives a DataArray<T> of atomic model.
    //    dynamics_id gives the correct library

    unsigned dynamics_id;
    ID model_id;
    bool debug;
};

template<typename T>
struct data_array_wrapper
{
    irr::data_array<T, ID> models;

    ID alloc()
    {
        auto& mdl = models.alloc();
        return models.get_id(mdl);
    }

    void free(ID model)
    {
        models.free(model);
    }

    int size() const noexcept
    {
        return models.size();
    }

    double start(ID model, double time)
    {
        auto* mdl = models.try_to_get(model);
        irr_assert(mdl);

        return mdl->start(time);
    }

    double transition(ID model, double time)
    {
        auto* mdl = models.try_to_get(model);
        irr_assert(mdl);

        return mdl->transition(time);
    }

    void output(ID model)
    {
        auto* mdl = models.try_to_get(model);
        irr_assert(mdl);

        return mdl->output();
    }
};

template<typename T>
struct AllDynamics
{
    std::vector<data_array_wrapper<T>>
      all_dynamics; // Initialized with the size of the
                    // DataArray<Dynamics>::size attribute.

    ID alloc(unsigned dynamics_id)
    {
        return (dynamics_id < all_dynamics.size())
                 ? all_dynamics[dynamics_id].alloc()
                 : 0;
    }

    void free(unsigned dynamics_id, ID model_id)
    {
        if (dynamics_id < all_dynamics.size() &&
            (model_id < all_dynamics[dynamics_id].size()))
            all_dynamics[dynamics_id].free(model_id);
    }
};

struct SimElement
{
    double tn;
    double tl;

    enum class simulator_element_type
    {
        simulator,
        coordinator
    };

    simulator_element_type type = simulator_element_type::simulator;
};

struct InternalEvent
{
    double time;
    ID id;
};

struct Simulation
{
    enum class simulation_type
    {
        devs,
        dsde
    };

    enum simulation_option
    {
        none = 0,
        flat = 1
    };

    double begin = 0.0;
    double end = 1.0;
    double current = 0.0;
    unsigned id = 0u;
    simulation_type type;
    unsigned option;
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

    Context(int verbose_level = 6);
    Context(std::FILE* f, int verbose_level = 6);

    std::FILE* cfile_logger = stdout;
    message_type log_priority = Context::message_type::info;
};

struct Vpz
{
    std::string name;
    std::string author;
    std::string version;
    std::string date;

    irr::data_array<Dynamics, ID> dynamics;
    irr::data_array<View, ID> views;
    irr::data_array<Value, ID> values;
    irr::data_array<NamedValue, ID> named_values;
    irr::data_array<Condition, ID> conditions;
    irr::data_array<Port, ID> input_ports;
    irr::data_array<Port, ID> output_ports;
    irr::data_array<Model, ID> models;
    irr::data_array<Class, ID> classes;

    irr::data_array<std::int32_t, ID> integer32;
    irr::data_array<std::int64_t, ID> integer64;
    irr::data_array<float, ID> real32;
    irr::data_array<double, ID> real64;
    irr::data_array<vec2, ID> vec2_32;
    irr::data_array<vec3, ID> vec3_32;

    irr::linker<ID, ID> value_links;
    irr::linker<ID, ID> named_value_links;
    irr::linker<ID, ID> model_dynamics;
    irr::linker<ID, ID> model_parent;
    irr::multi_linker<ID, ID> model_input_ports;
    irr::multi_linker<ID, ID> model_output_ports;
    irr::multi_linker<ID, ID2> model_connections;
    irr::multi_linker<ID, ID> model_conditions;
    irr::multi_linker<ID, ID> model_children;
    irr::multi_linker<ID, ID> condition_named_values;

    void init(int object_capacity, int list_capacity);

    Dynamics& add_local_dynamics(std::string name, std::string library);
    void destroy(Dynamics& dynamic);
    View& add_timed_view(std::string name,
                         std::string location,
                         double timestep,
                         View::output_type type);
    View& add_event_view(std::string name,
                         std::string location,
                         unsigned view,
                         View::output_type type);
    void destroy(View& view);
    Model& add_model(std::string name, Model::model_type type);
    void make_parent(const Model& model, const Model& parent);
    void destroy(Model& model);

    Value& add_value();
    Value& add_value(float value);
    Value& add_value(double value);
    Value& add_value(std::int32_t value);
    Value& add_value(std::int64_t value);
    Value& add_value(float x, float y);
    Value& add_value(float x, float y, float z);
    void destroy(Value& value);

    void destroy(NamedValue& value);
    NamedValue& add_named_value(std::string name);
    NamedValue& add_named_value(std::string name, float value);
    NamedValue& add_named_value(std::string name, double value);
    NamedValue& add_named_value(std::string name, std::int32_t value);
    NamedValue& add_named_value(std::string name, std::int64_t value);
    NamedValue& add_named_value(std::string name, float x, float y);
    NamedValue& add_named_value(std::string name, float x, float y, float z);

    Condition& add_condition(std::string name);
    void append(const Condition& condition, NamedValue& value);
    void destroy(Condition& condition);

    Port& add_input_port(const Model& model, std::string name);
    void destroy_input_port(Port& port);

    Port& add_output_port(const Model& model, std::string name);
    void destroy_output_port(Port& port);

    void make_connection(const Model& coupledmodel,
                         const Port& port_source,
                         const Port& port_destination);
};

struct VLE
{
    std::array<std::string, 5> paths;
};

VLE
init_vle() noexcept;

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_SIMULATION_HPP
