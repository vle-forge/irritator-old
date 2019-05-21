// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/simulation.hpp>

#include <rapidjson/filereadstream.h>
#include <rapidjson/reader.h>

#include <vector>

#include "private.hpp"

#include <iostream>

enum class irr_element : std::int8_t
{
    none,
    project,
    project_name,
    project_author,
    project_version,
    conditions,
    conditions_array,
    conditions_array_object,
    views,
    views_array,
    views_array_object,
    views_array_object_values,
    views_array_object_values_options,
    views_array_object_values_type,
    views_array_object_values_conditions
};

static constexpr std::int8_t irr_element_size = 15;

static constexpr std::string_view irr_element_name[irr_element_size] = {
    "none",
    "project",
    "project_name",
    "project_author",
    "project_version",
    "conditions",
    "conditions_array",
    "conditions_array_object",
    "views",
    "views_array",
    "views_array_object",
    "views_array_object_values",
    "views_array_object_values_options",
    "views_array_object_values_type",
    "views_array_object_values_conditions"
};

struct irr_stack_element
{
    irr_element element = irr_element::none;
    irr::ID id = 0;

    constexpr irr_stack_element(const irr_element element_) noexcept
      : element(element_)
    {}

    constexpr irr_stack_element(const irr_element element_,
                                const irr::ID id_) noexcept
      : element(element_)
      , id(id_)
    {}

    constexpr bool is_project() const noexcept
    {
        return element == irr_element::project;
    }

    constexpr bool is_project_name() const noexcept
    {
        return element == irr_element::project_name;
    }

    constexpr bool is_project_author() const noexcept
    {
        return element == irr_element::project_author;
    }

    constexpr bool is_project_version() const noexcept
    {
        return element == irr_element::project_version;
    }

    constexpr bool is_conditions() const noexcept
    {
        return element == irr_element::conditions;
    }

    constexpr bool is_conditions_array() const noexcept
    {
        return element == irr_element::conditions_array;
    }

    constexpr bool is_conditions_array_object() const noexcept
    {
        return element == irr_element::conditions_array_object;
    }

    constexpr bool is_views() const noexcept
    {
        return element == irr_element::views;
    }

    constexpr bool is_views_array() const noexcept
    {
        return element == irr_element::views_array;
    }

    constexpr bool is_views_array_object() const noexcept
    {
        return element == irr_element::views_array_object;
    }

    constexpr bool is_views_array_object_values() const noexcept
    {
        return element == irr_element::views_array_object_values;
    }

    constexpr bool is_views_array_object_values_options() const noexcept
    {
        return element == irr_element::views_array_object_values_options;
    }

    constexpr bool is_views_array_object_values_type() const noexcept
    {
        return element == irr_element::views_array_object_values_type;
    }

    constexpr bool is_views_array_object_values_conditions() const noexcept
    {
        return element == irr_element::views_array_object_values_conditions;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const irr_stack_element& elem)
    {
        const auto index =
          static_cast<std::underlying_type<irr_element>::type>(elem.element);

        assert(index > 0 && index < irr_element_size);

        return os << "[" << irr_element_name[index] << "]";
    }
};

struct irr_json_stack
{
    std::vector<irr_stack_element> stack;

    friend std::ostream& operator<<(std::ostream& os,
                                    const irr_json_stack& stack)
    {
        std::copy(stack.stack.rbegin(),
                  stack.stack.rend(),
                  std::ostream_iterator<irr_stack_element>(os));

        return os;
    }

    irr_json_stack()
    {
        stack.reserve(1024);
    }

    bool empty() const noexcept
    {
        return stack.empty();
    }

    irr_stack_element& top() noexcept
    {
        return stack.back();
    }

    irr_stack_element top() const noexcept
    {
        return stack.back();
    }

    template<typename... Args>
    void emplace(Args&&... args) noexcept
    {
        stack.emplace_back(std::forward<Args>(args)...);

        std::cout << "emplace stack: " << *this << '\n';
    }

    irr_stack_element pop() noexcept
    {
        auto ret = top();
        stack.pop_back();

        std::cout << "popped stack: " << *this << '\n';

        return ret;
    }
};

struct irr_json_handler
  : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, irr_json_handler>
{
    irr_json_stack stack;
    irr::Model& model;

    irr_json_handler(irr::Model& model_)
      : model(model_)
    {}

    bool Null()
    {
        std::cout << "Null()" << std::endl;
        return true;
    }

    bool Bool(bool b)
    {
        std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl;

        if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(b ? 1 : 0);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Int(int i)
    {
        std::cout << "Int(" << i << ")" << std::endl;

        if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(i);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Uint(unsigned u)
    {
        std::cout << "Uint(" << u << ")" << std::endl;

        if (stack.top().is_project_version()) {
            if (model.version_major < 0)
                model.version_major = static_cast<int>(u);
            else if (model.version_minor < 0)
                model.version_minor = static_cast<int>(u);
            else if (model.version_patch < 0)
                model.version_patch = static_cast<int>(u);
            // stack.pop() will occurs in EndArray function.
        } else if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(u);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Int64(int64_t i)
    {
        std::cout << "Int64(" << i << ")" << std::endl;

        if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer64s.alloc(i);
            cnd->value = model.integer64s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer64;
        }

        return true;
    }

    bool Uint64(uint64_t u)
    {
        std::cout << "Uint64(" << u << ")" << std::endl;

        if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer64s.alloc(u);
            cnd->value = model.integer64s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer64;
        }

        return true;
    }

    bool Double(double d)
    {
        std::cout << "Double(" << d << ")" << std::endl;

        if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.real64s.alloc(d);
            cnd->value = model.real64s.get_id(value);
            cnd->type = irr::Condition::condition_type::real64;
        }

        return true;
    }

    bool RawNumber(const char* str, rapidjson::SizeType length, bool copy)
    {
        std::cout << "Number(" << str << ", " << length << ", "
                  << std::boolalpha << copy << ")" << std::endl;
        return true;
    }

    bool String(const char* str, rapidjson::SizeType length, bool copy)
    {
        std::cout << "String(" << str << ", " << length << ", "
                  << std::boolalpha << copy << ")" << std::endl;

        auto elem = stack.top();

        if (elem.element == irr_element::project_name) {
            model.name = str;
            stack.pop();
        } else if (elem.element == irr_element::project_author) {
            model.author = str;
            stack.pop();
        } else if (stack.top().is_conditions_array_object()) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.strings.alloc(str, length);
            cnd->value = model.strings.get_id(value);
            cnd->type = irr::Condition::condition_type::string;
        } else if (stack.top().is_views_array_object_values_options()) {
            std::int8_t option = 0;

            if (!strncmp(str, "alloc", length))
                option |= irr::View::view_option::alloc;
            else if (!strncmp(str, "output", length))
                option |= irr::View::view_option::output;
            else if (!strncmp(str, "internal", length))
                option |= irr::View::view_option::internal;
            else if (!strncmp(str, "external", length))
                option |= irr::View::view_option::external;
            else if (!strncmp(str, "confluent", length))
                option |= irr::View::view_option::confluent;
            else if (!strncmp(str, "finish", length))
                option |= irr::View::view_option::finish;

            auto* view = model.views.try_to_get(stack.top().id);
            view->options = option;
            // stack.pop() will occurred in the EndArray.
        } else if (stack.top().is_views_array_object_values_conditions()) {
            auto* view = model.views.try_to_get(stack.top().id);
            irr::Condition* cnd = nullptr;

            while (model.conditions.next(cnd))
                if (cnd->name == str)
                    break;

            if (cnd) {
                view->options = 0; // TODO remove ! only for remove warning
                // stack.pop() will occurred in the EndArray.
                // view->conditions.emplace_back(model.condition.get_id(*cnd));
                return true;
            } else {
                // context->error("unknown condition {}", str);
                return false;
            }
        }

        return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy)
    {
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha
                  << copy << ")" << std::endl;

        if (stack.top().is_project()) {
            if (!strncmp(str, "name", length))
                stack.emplace(irr_element::project_name);
            else if (!strncmp(str, "author", length))
                stack.emplace(irr_element::project_author);
            else if (!strncmp(str, "version", length))
                stack.emplace(irr_element::project_version);
            else if (!strncmp(str, "conditions", length))
                stack.emplace(irr_element::conditions);
            else if (!strncmp(str, "views", length))
                stack.emplace(irr_element::views);
        } else if (stack.top().is_conditions_array_object()) {
            auto& condition = model.conditions.alloc(str);
            auto id = model.conditions.get_id(condition);
            std::cout << "read condition " << condition.name.data() << '\n';
            stack.top().id = id;
        } else if (stack.top().is_views_array_object()) {
            auto& view = model.views.alloc(str);
            auto id = model.views.get_id(view);
            std::cout << "read view " << view.name.data() << '\n';
            stack.emplace(irr_element::views_array_object_values, id);
        } else if (stack.top().is_views_array_object_values()) {
            if (!strncmp(str, "options", length)) {
                stack.emplace(irr_element::views_array_object_values_options,
                              stack.top().id);
            } else if (!strncmp(str, "type", length)) {
                stack.emplace(irr_element::views_array_object_values_type,
                              stack.top().id);
            } else if (!strncmp(str, "conditions", length)) {
                stack.emplace(
                  irr_element::views_array_object_values_conditions,
                  stack.top().id);
            }
        }

        return true;
    }

    bool StartObject()
    {
        std::cout << "StartObject()" << std::endl;

        if (stack.empty()) {
            stack.emplace(irr_element::project);
        } else if (stack.top().is_conditions_array()) {
            stack.emplace(irr_element::conditions_array_object);
        } else if (stack.top().is_views_array()) {
            stack.emplace(irr_element::views_array_object);
        }

        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount)
    {
        std::cout << "EndObject(" << memberCount << ")" << std::endl;

        stack.pop();

        return true;
    }

    bool StartArray()
    {
        std::cout << "StartArray()" << std::endl;

        if (stack.top().is_project_version()) {
            model.version_major = -1;
            model.version_minor = -1;
            model.version_patch = -1;
        } else if (stack.top().is_conditions()) {
            stack.emplace(irr_element::conditions_array);
        } else if (stack.top().is_views()) {
            stack.emplace(irr_element::views_array);
        } else if (stack.top().is_views_array_object_values()) {
        }

        return true;
    }

    bool EndArray(rapidjson::SizeType elementCount)
    {
        std::cout << "EndArray(" << elementCount << ")" << std::endl;

        stack.pop();

        return true;
    }
};

namespace irr {

void
Model::read(Context& context, const std::filesystem::path& file_name)
{
    auto* fd = std::fopen(file_name.c_str(), "r");
    if (!fd) {
        debug(context, "Fail to open file {}\n", file_name.string());
        return;
    }

    char buffer[4096];
    rapidjson::FileReadStream is(fd, buffer, sizeof(buffer));

    irr_json_handler handler(*this);
    rapidjson::Reader reader;

    if (!reader.Parse(is, handler)) {
        error(context, "Fail to read file {}\n", file_name.string());
        return;
    }
}

} // namespace irr
