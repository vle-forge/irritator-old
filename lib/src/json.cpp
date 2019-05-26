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
    conditions_object,
    views,
    views_object,
    views_object_parameters,
    views_object_parameters_options,
    views_object_parameters_type,
    views_object_parameters_conditions,
    COUNT
};

static constexpr std::string_view irr_element_name[static_cast<std::int8_t>(
  irr_element::COUNT)] = { "none",
                           "project",
                           "project_name",
                           "project_author",
                           "project_version",
                           "conditions",
                           "conditions_object",
                           "views",
                           "views_object",
                           "views_object_parameters",
                           "views_object_parameters_options",
                           "views_object_parameters_type",
                           "views_object_parameters_conditions" };

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

    constexpr bool is(const irr_element compare_element) const noexcept
    {
        return element == compare_element;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const irr_stack_element& elem)
    {
        const auto index =
          static_cast<std::underlying_type<irr_element>::type>(elem.element);

        assert(index > 0 &&
               index < static_cast<std::underlying_type<irr_element>::type>(
                         irr_element::COUNT));

        return os << "[" << irr_element_name[index] << "]";
    }
};

struct irr_json_stack
{
    std::vector<irr_stack_element> stack;

    friend std::ostream& operator<<(std::ostream& os,
                                    const irr_json_stack& stack) noexcept
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

    auto size() const noexcept
    {
        return stack.size();
    }

    irr_stack_element& top() noexcept
    {
        assert(!empty());
        return stack.back();
    }

    irr_stack_element top() const noexcept
    {
        assert(!empty());
        return stack.back();
    }

    template<typename... Args>
    void emplace(Args&&... args) noexcept
    {
        stack.emplace_back(std::forward<Args>(args)...);
        // std::cout << "emplace stack: " << *this << '\n';
    }

    irr_stack_element pop() noexcept
    {
        assert(!empty());
        auto ret = top();
        stack.pop_back();

        return ret;
    }
};

struct irr_json_handler
  : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, irr_json_handler>
{
    irr_json_stack stack;
    irr::Model& model;
    irr::Context& context;

    void indent() noexcept
    {
        fmt::print("{:>{}}", "", stack.size());
    }

    irr_json_handler(irr::Model& model_, irr::Context& context_)
      : model(model_)
      , context(context_)
    {}

    bool Null()
    {
        indent();
        std::cout << "Null()" << std::endl;
        stack.pop();
        return true;
    }

    bool Bool(bool b)
    {
        indent();
        std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl;

        if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(b ? 1 : 0);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Int(int i)
    {
        indent();
        std::cout << "Int(" << i << ")" << std::endl;

        if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(i);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Uint(unsigned u)
    {
        indent();
        std::cout << "Uint(" << u << ")" << std::endl;

        if (stack.top().is(irr_element::project_version)) {
            if (model.version_major < 0)
                model.version_major = static_cast<int>(u);
            else if (model.version_minor < 0)
                model.version_minor = static_cast<int>(u);
            else if (model.version_patch < 0)
                model.version_patch = static_cast<int>(u);
        } else if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer32s.alloc(u);
            cnd->value = model.integer32s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer32;
        }

        return true;
    }

    bool Int64(int64_t i)
    {
        indent();
        std::cout << "Int64(" << i << ")" << std::endl;

        if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer64s.alloc(i);
            cnd->value = model.integer64s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer64;
        }

        return true;
    }

    bool Uint64(uint64_t u)
    {
        indent();
        std::cout << "Uint64(" << u << ")" << std::endl;

        if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.integer64s.alloc(u);
            cnd->value = model.integer64s.get_id(value);
            cnd->type = irr::Condition::condition_type::integer64;
        }

        return true;
    }

    bool Double(double d)
    {
        indent();
        std::cout << "Double(" << d << ")" << std::endl;

        if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.real64s.alloc(d);
            cnd->value = model.real64s.get_id(value);
            cnd->type = irr::Condition::condition_type::real64;
        }

        return true;
    }

    bool RawNumber(const char* str, rapidjson::SizeType length, bool copy)
    {
        indent();
        std::cout << "Number(" << str << ", " << length << ", "
                  << std::boolalpha << copy << ")" << std::endl;
        return true;
    }

    bool String(const char* str, rapidjson::SizeType length, bool copy)
    {
        indent();
        std::cout << "String(" << str << ", " << length << ", "
                  << std::boolalpha << copy << ")" << std::endl;

        auto elem = stack.top();

        if (elem.element == irr_element::project_name) {
            model.name = str;
            stack.pop();
        } else if (elem.element == irr_element::project_author) {
            model.author = str;
            stack.pop();
        } else if (stack.top().is(irr_element::conditions_object)) {
            auto* cnd = model.conditions.try_to_get(stack.top().id);
            auto& value = model.strings.alloc(str, length);
            cnd->value = model.strings.get_id(value);
            cnd->type = irr::Condition::condition_type::string;
        } else if (stack.top().is(
                     irr_element::views_object_parameters_options)) {
            auto* view = model.views.try_to_get(stack.top().id);
            if (!strncmp(str, "alloc", length))
                view->options |= irr::View::view_option::alloc;
            else if (!strncmp(str, "output", length))
                view->options |= irr::View::view_option::output;
            else if (!strncmp(str, "internal", length))
                view->options |= irr::View::view_option::internal;
            else if (!strncmp(str, "external", length))
                view->options |= irr::View::view_option::external;
            else if (!strncmp(str, "confluent", length))
                view->options |= irr::View::view_option::confluent;
            else if (!strncmp(str, "finish", length))
                view->options |= irr::View::view_option::finish;
        } else if (stack.top().is(irr_element::views_object_parameters_type)) {
            auto* view = model.views.try_to_get(stack.top().id);
            if (!strncmp(str, "csv_file", length))
                view->type = irr::View::view_type::csv_file;
            else if (!strncmp(str, "json_file", length))
                view->type = irr::View::view_type::json_file;
            else if (!strncmp(str, "memory", length))
                view->type = irr::View::view_type::memory;
        } else if (stack.top().is(
                     irr_element::views_object_parameters_conditions)) {
            auto* view = model.views.try_to_get(stack.top().id);
            irr::Condition* cnd = nullptr;

            while (model.conditions.next(cnd))
                if (cnd->name == str)
                    break;

            if (cnd) {
                view->conditions.push_back(model.links,
                                           model.conditions.get_id(*cnd));
                return true;
            } else {
                info(context, "unknown condition {} - adding it", str);
                auto& condition = model.conditions.alloc(str);
                auto id = model.conditions.get_id(condition);
                view->conditions.push_back(model.links, id);
            }
        }

        return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy)
    {
        indent();
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha
                  << copy << ") " << stack << std::endl;

        if (stack.top().is(irr_element::project)) {
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
        } else if (stack.top().is(irr_element::conditions_object)) {
            auto& condition = model.conditions.alloc(str);
            auto id = model.conditions.get_id(condition);
            std::cout << "** new condition " << condition.name.data() << '\n';
            stack.top().id = id;
        } else if (stack.top().is(irr_element::views_object)) {
            auto& view = model.views.alloc(str);
            auto id = model.views.get_id(view);
            std::cout << "** new view " << view.name.data() << ' ' << id
                      << '\n';
            stack.top().id = id;
            stack.emplace(irr_element::views_object_parameters, id);
        } else if (stack.top().is(irr_element::views_object_parameters)) {
            if (!strncmp(str, "options", length)) {
                stack.emplace(irr_element::views_object_parameters_options,
                              stack.top().id);
            } else if (!strncmp(str, "type", length)) {
                stack.emplace(irr_element::views_object_parameters_type,
                              stack.top().id);
            } else if (!strncmp(str, "conditions", length)) {
                stack.emplace(irr_element::views_object_parameters_conditions,
                              stack.top().id);
            }
        }

        return true;
    }

    bool StartObject()
    {
        indent();
        std::cout << "StartObject() " << stack << std::endl;

        if (stack.empty()) {
            stack.emplace(irr_element::project);
        } else if (stack.top().is(irr_element::conditions)) {
            stack.emplace(irr_element::conditions_object);
        } else if (stack.top().is(irr_element::views)) {
            stack.emplace(irr_element::views_object);
        }

        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount)
    {
        indent();
        std::cout << "EndObject(" << memberCount << ") ";

        std::cout << "Pop: " << stack.pop();
        std::cout << " stack: " << stack << "\n";

        return true;
    }

    bool StartArray()
    {
        indent();
        std::cout << "StartArray()" << stack << std::endl;

        if (stack.top().is(irr_element::project_version)) {
            model.version_major = -1;
            model.version_minor = -1;
            model.version_patch = -1;
        }

        return true;
    }

    bool EndArray(rapidjson::SizeType elementCount)
    {
        indent();

        std::cout << "EndArray(" << elementCount << ") ";

        switch (stack.top().element) {
        case irr_element::views_object:
        case irr_element::views_object_parameters_options:
        case irr_element::views_object_parameters_conditions:
            std::cout << "\n";
            break;
        default:
            std::cout << "Pop: " << stack.pop();
            std::cout << " stack: " << stack << "\n";
        }

        // if (!(stack.top().is(irr_element::views_object) ||
        //       stack.top().is(irr_element::views))) {
        //     std::cout << "Pop: " << stack.pop();
        //     std::cout << " stack: " << stack << "\n";
        // }

        return true;
    }
};

namespace irr {

status
Model::read(Context& context, const std::filesystem::path& file_name)
{
    auto* fd = std::fopen(file_name.c_str(), "r");
    if (!fd) {
        debug(context, "std::fopen error: {} {}\n", errno, ::strerror(errno));
        return irr::status::json_open_error;
    }

    char buffer[4096];
    rapidjson::FileReadStream is(fd, buffer, sizeof(buffer));

    irr_json_handler handler(*this, context);
    rapidjson::Reader reader;

    if (!reader.Parse(is, handler))
        return irr::status::json_parse_error;

    if (!handler.stack.empty())
        return irr::status::json_stack_not_empty_error;

    return irr::status::json_read_success;
}

} // namespace irr
