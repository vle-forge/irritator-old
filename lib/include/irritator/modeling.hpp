// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_MODELING_HPP
#define ORG_VLEPROJECT_IRRITATOR_MODELING_HPP

#include <irritator/data-array.hpp>
#include <irritator/data-list.hpp>
#include <irritator/export.hpp>
#include <irritator/string.hpp>

#include <filesystem>
#include <memory>
#include <optional>

namespace irr {

constexpr int max_name_length = 8;

struct Condition
{
    Condition() = default;

    Condition(const char* str)
      : value(0)
      , type(condition_type::integer32)
      , name(str)
    {}

    enum class condition_type : int8_t
    {
        integer32, // int32_t
        integer64, // int64_t
        real64,    // double
        string,    // std::string
        // set,       // std::vector<named_value>
        // map        // std::map<string, named_value>
    };

    ID value = 0;
    condition_type type = condition_type::integer32;
    string<3 + 4> name;
};

// struct GuiNode
// {
//     float x = 0.f;
//     float y = 0.f;
//     float height = 0.f;
//     float width = 0.f;
//     bool selected = false;
// };

struct Connection
{
    ID input_model = 0;
    ID output_model = 0;
    ID input_slot = 0;
    ID output_slot = 0;
};

// struct GuiSlot
// {
//     vec2 position;
// };

struct Slot
{
    string<8> name;
};

// struct GuiConnection
// {
//     vec2 point[3];
// };

struct Dynamic
{
    string<8> name;

    string<1024> package;
    string<1024> library;
};

struct View
{
    View() = default;

    View(const char* str)
      : name(str)
    {}

    enum view_option : std::int8_t
    {
        none = 0,
        alloc = 1 << 1,
        output = 1 << 2,
        internal = 1 << 3,
        external = 1 << 4,
        confluent = 1 << 5,
        finish = 1 << 6,
    };

    enum class view_type : std::int8_t
    {
        csv_file,
        json_file,
        memory
    };

    ListID conditions;
    std::int8_t options = view_option::output;
    view_type type = view_type::csv_file;
    string<2 + 4> name;
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
    ListID conditions;
    ListID observables;

    ListID children;
    ListID connections;

    model_type type = model_type::atomic;
};

struct Class
{
    ID name;
    ID model;
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

using Conditions = data_array<Condition, ID>;

using Integer32s = data_array<int32_t, ID>;
using Integer64s = data_array<int64_t, ID>;
using Real32s = data_array<float, ID>;
using Real64s = data_array<double, ID>;
using Strings = data_array<std::string, ID>;

using Nodes = data_array<Node, ID>;
using Connections = data_array<Connection, ID>;
using Slots = data_array<Slot, ID>;
using Views = data_array<View, ID>;
using Dynamics = data_array<Dynamic, ID>;
using Classes = data_array<Class, ID>;

enum class status
{
    json_read_success,
    json_open_error,
    json_parse_error,
    json_stack_not_empty_error
};

struct Model
{
    Model(int estimated_model_number = 4096);

    status read(Context& context, const std::filesystem::path& file_name);

    string<32> name;
    string<32> author;
    int version_major;
    int version_minor;
    int version_patch;

    Conditions conditions;
    Connections connections;
    Slots slots;
    Views views;
    Nodes nodes;
    Classes classes;
    Dynamics dynamics;

    data_list<ID> links;
    data_list<WID> wlinks;

    Integer32s integer32s;
    Integer64s integer64s;
    Real32s real32s;
    Real64s real64s;
    Strings strings;
};

struct VLE
{
    VLE();

    std::array<std::filesystem::path, 4> paths;
    Context context;
    Dynamics dynamics;
};

} // irr

#endif // ORG_VLEPROJECT_IRRITATOR_MODELING_HPP
