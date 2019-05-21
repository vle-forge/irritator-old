// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/simulation.hpp>

#include "private.hpp"

namespace irr {

// void
// Vpz::init(int object_capacity, int /*list_capacity*/)
// {
//     dynamics.init(object_capacity);
//     views.init(object_capacity);
//     conditions.init(object_capacity);
//     input_ports.init(object_capacity);
//     output_ports.init(object_capacity);
//     models.init(object_capacity);
//     classes.init(object_capacity);
//     values.init(object_capacity);
//     named_values.init(object_capacity);

//     integer32.init(object_capacity);
//     integer64.init(object_capacity);
//     real32.init(object_capacity);
//     real64.init(object_capacity);
//     vec2_32.init(object_capacity);
//     vec3_32.init(object_capacity);
// }

// Dynamics&
// Vpz::add_local_dynamics(std::string name, std::string library)
// {
//     Dynamics& ret = dynamics.alloc();

//     ret.name = std::move(name);
//     ret.library = std::move(library);
//     ret.type = Dynamics::dynamics_type::local;

//     return ret;
// }

// void
// Vpz::destroy(Dynamics& dynamic)
// {
//     dynamics.free(dynamic);
// }

// View&
// Vpz::add_timed_view(std::string name,
//                     std::string location,
//                     double timestep,
//                     View::output_type type)
// {
//     View& ret = views.alloc();

//     ret.name = std::move(name);
//     ret.location = std::move(location);
//     ret.timestep = timestep;
//     ret.view = View::view_option::timed;
//     ret.type = type;

//     return ret;
// }

// View&
// Vpz::add_event_view(std::string name,
//                     std::string location,
//                     unsigned view,
//                     View::output_type type)
// {
//     View& ret = views.alloc();

//     ret.name = std::move(name);
//     ret.location = std::move(location);
//     ret.view = view;
//     ret.type = type;

//     return ret;
// }

// void
// Vpz::destroy(View& view)
// {
//     views.free(view);
// }

// Model&
// Vpz::add_model(std::string name, Model::model_type type)
// {
//     Model& ret = models.alloc();

//     ret.name = name;
//     ret.type = type;

//     return ret;
// }

// void
// Vpz::make_parent(const Model& model, const Model& parent)
// {
//     irr_assert(parent.type == Model::model_type::coupled);

//     auto id_model = models.get_id(model);
//     auto id_parent = models.get_id(parent);

//     model_parent[id_model] = id_parent;
//     model_children.emplace(id_parent, id_model);
// }

// void
// Vpz::destroy(Model& model)
// {
//     auto id_model = models.get_id(model);

//     for (auto& port : model_input_ports.get_view(id_model, input_ports))
//         input_ports.free(port);

//     for (auto& port : model_output_ports.get_view(id_model, output_ports))
//         output_ports.free(port);

//     model_output_ports.destroy(id_model);
//     model_output_ports.destroy(id_model);

//     if (model.type == Model::model_type::coupled) {
//         for (auto& mdl : model_children.get_view(id_model, models))
//             destroy(mdl);

//         model_children.destroy(id_model);
//         model_connections.destroy(id_model);
//     }

//     model_dynamics.destroy(id_model);
//     model_parent.destroy(id_model);
//     model_conditions.destroy(id_model);
//     models.free(id_model);
// }

// Value&
// Vpz::add_value()
// {
//     auto& v = values.alloc();
//     v.type = value_type::none;

//     return v;
// }

// Value&
// Vpz::add_value(float value)
// {
//     auto& v = values.alloc();
//     v.type = value_type::real32;

//     auto& x = real32.alloc();
//     x = value;
//     value_links.emplace(values.get_id(v), real32.get_id(x));

//     return v;
// }

// Value&
// Vpz::add_value(double value)
// {
//     auto& v = values.alloc();
//     v.type = value_type::real64;

//     auto& x = real64.alloc();
//     x = value;
//     value_links.emplace(values.get_id(v), real64.get_id(x));

//     return v;
// }

// Value&
// Vpz::add_value(std::int32_t value)
// {
//     auto& v = values.alloc();
//     v.type = value_type::integer32;

//     auto& x = integer32.alloc();
//     x = value;
//     value_links.emplace(values.get_id(v), integer32.get_id(x));

//     return v;
// }

// Value&
// Vpz::add_value(std::int64_t value)
// {
//     auto& v = values.alloc();
//     v.type = value_type::integer64;

//     auto& x = integer64.alloc();
//     x = value;
//     value_links.emplace(values.get_id(v), integer64.get_id(x));

//     return v;
// }

// Value&
// Vpz::add_value(float x, float y)
// {
//     auto& v = values.alloc();
//     v.type = value_type::vec2_32;

//     auto& vec = vec2_32.alloc();
//     vec.x = x;
//     vec.y = y;
//     value_links.emplace(values.get_id(v), vec2_32.get_id(vec));

//     return v;
// }

// Value&
// Vpz::add_value(float x, float y, float z)
// {
//     auto& v = values.alloc();
//     v.type = value_type::vec3_32;

//     auto& vec = vec3_32.alloc();
//     vec.x = x;
//     vec.y = y;
//     vec.z = z;
//     value_links.emplace(values.get_id(v), vec3_32.get_id(vec));

//     return v;
// }

// void
// Vpz::destroy(Value& value)
// {
//     switch (value.type) {
//     case value_type::integer32: {
//         auto* x = integer32.try_to_get(value_links[values.get_id(value)]);
//         integer32.free(*x);
//     } break;
//     case value_type::integer64: {
//         auto* x = integer64.try_to_get(value_links[values.get_id(value)]);
//         integer64.free(*x);
//     } break;
//     case value_type::real32: {
//         auto* x = real32.try_to_get(value_links[values.get_id(value)]);
//         real32.free(*x);
//     } break;
//     case value_type::real64: {
//         auto* x = real64.try_to_get(value_links[values.get_id(value)]);
//         real64.free(*x);
//     } break;
//     case value_type::vec2_32: {
//         auto* x = vec2_32.try_to_get(value_links[values.get_id(value)]);
//         vec2_32.free(*x);
//     } break;
//     case value_type::vec3_32: {
//         auto* x = vec3_32.try_to_get(value_links[values.get_id(value)]);
//         vec3_32.free(*x);
//     } break;
//     case value_type::none:
//         break;
//     default:
//         break;
//     }
// }

// void
// Vpz::destroy(NamedValue& value)
// {
//     switch (value.type) {
//     case value_type::integer32: {
//         auto* x =
//           integer32.try_to_get(named_value_links[named_values.get_id(value)]);
//         integer32.free(*x);
//     } break;
//     case value_type::integer64: {
//         auto* x =
//           integer64.try_to_get(named_value_links[named_values.get_id(value)]);
//         integer64.free(*x);
//     } break;
//     case value_type::real32: {
//         auto* x =
//           real32.try_to_get(named_value_links[named_values.get_id(value)]);
//         real32.free(*x);
//     } break;
//     case value_type::real64: {
//         auto* x =
//           real64.try_to_get(named_value_links[named_values.get_id(value)]);
//         real64.free(*x);
//     } break;
//     case value_type::vec2_32: {
//         auto* x =
//           vec2_32.try_to_get(named_value_links[named_values.get_id(value)]);
//         vec2_32.free(*x);
//     } break;
//     case value_type::vec3_32: {
//         auto* x =
//           vec3_32.try_to_get(named_value_links[named_values.get_id(value)]);
//         vec3_32.free(*x);
//     } break;
//     case value_type::none:
//         break;
//     default:
//         break;
//     }
// }

// NamedValue&
// Vpz::add_named_value(std::string name)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::none;

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, float value)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::real32;

//     auto& x = real32.alloc();
//     x = value;
//     named_value_links.emplace(named_values.get_id(v), real32.get_id(x));

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, double value)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::real64;

//     auto& x = real64.alloc();
//     x = value;
//     named_value_links.emplace(named_values.get_id(v), real64.get_id(x));

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, std::int32_t value)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::integer32;

//     auto& x = integer32.alloc();
//     x = value;
//     named_value_links.emplace(named_values.get_id(v), integer32.get_id(x));

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, std::int64_t value)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::integer64;

//     auto& x = integer64.alloc();
//     x = value;
//     named_value_links.emplace(named_values.get_id(v), integer64.get_id(x));

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, float x, float y)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::vec2_32;

//     auto& vec = vec2_32.alloc();
//     vec.x = x;
//     vec.y = y;
//     named_value_links.emplace(named_values.get_id(v), vec2_32.get_id(vec));

//     return v;
// }

// NamedValue&
// Vpz::add_named_value(std::string name, float x, float y, float z)
// {
//     auto& v = named_values.alloc();
//     v.name = std::move(name);
//     v.type = value_type::vec3_32;

//     auto& vec = vec3_32.alloc();
//     vec.x = x;
//     vec.y = y;
//     vec.z = z;
//     named_value_links.emplace(named_values.get_id(v), vec3_32.get_id(vec));

//     return v;
// }

// Condition&
// Vpz::add_condition(std::string name)
// {
//     auto& cond = conditions.alloc();
//     cond.name = name;

//     return cond;
// }

// void
// Vpz::append(const Condition& condition, NamedValue& value)
// {
//     auto id = conditions.get_id(condition);
//     auto named_value_id = named_values.get_id(value);

//     condition_named_values.emplace(id, named_value_id);
// }

// void
// Vpz::destroy(Condition& condition)
// {
//     auto id = conditions.get_id(condition);

//     for (auto& nv : condition_named_values.get_view(id, named_values))
//         destroy(nv);

//     condition_named_values.destroy(id);
// }

// Port&
// Vpz::add_input_port(const Model& model, std::string name)
// {
//     Port& ret = input_ports.alloc();
//     ret.name = name;

//     auto id_model = models.get_id(model);

//     model_input_ports.emplace(id_model, input_ports.get_id(ret));

//     return ret;
// }

// Port&
// Vpz::add_output_port(const Model& model, std::string name)
// {
//     Port& ret = output_ports.alloc();
//     ret.name = name;

//     auto id_model = models.get_id(model);

//     model_output_ports.emplace(id_model, output_ports.get_id(ret));

//     return ret;
// }

// void
// Vpz::destroy_input_port(Port& port)
// {
//     input_ports.free(port);
// }

// void
// Vpz::destroy_output_port(Port& port)
// {
//     input_ports.free(port);
// }

// void
// Vpz::make_connection(const Model& coupledmodel,
//                      const Port& port_source,
//                      const Port& port_destination)
// {
//     auto id_coupledmodel = models.get_id(coupledmodel);
//     auto id_port_source = output_ports.get_id(port_source);
//     auto id_port_destination = input_ports.get_id(port_destination);

//     model_connections.emplace(id_coupledmodel,
//                               ID2(id_port_source, id_port_destination));
// }

Model::Model(int estimated_model_number)
{
    conditions.init(estimated_model_number);
    connections.init(estimated_model_number * 4);
    slots.init(estimated_model_number * 4);
    views.init(estimated_model_number);
    nodes.init(estimated_model_number);

    integer32s.init(estimated_model_number);
    integer64s.init(estimated_model_number);
    real64s.init(estimated_model_number);
    strings.init(estimated_model_number);
}

VLE::VLE()
{
    int value = 0;

    if (auto a = std::filesystem::path{ "/home/gquesnel/.vle3/packages" };
        std::filesystem::is_directory(a)) {
        paths[value++] = a;
    } else {
        debug(context, "{} is not a packages directory.\n", a.string());
    }

    if (auto a = std::filesystem::path{ "/usr/lib/vle3/packages" };
        std::filesystem::is_directory(a)) {
        paths[value++] = a;
    } else {
        debug(context, "{} is not a packages directory.\n", a.string());
    }

    if (auto a = std::filesystem::path{ "/usr/local/lib/vle3/packages" };
        std::filesystem::is_directory(a)) {
        paths[value++] = a;
    } else {
        debug(context, "{} is not a packages directory.\n", a.string());
    }

    if (auto a = std::filesystem::path{ "/opt/vle3/lib/vle3/packages" };
        std::filesystem::is_directory(a)) {
        paths[value++] = a;
    } else {
        debug(context, "{} is not a packages directory.\n", a.string());
    }

    if (value == 0)
        error(context, "Fail to found dynamics in any directory.\n");
}
};
