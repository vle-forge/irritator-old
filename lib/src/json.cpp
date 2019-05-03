// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <irritator/simulation.hpp>

#include <rapidjson/filereadstream.h>
#include <rapidjson/reader.h>

#include <vector>

#include "private.hpp"

#include <iostream>

enum class irr_json_element_type
{
    none,
    project,
    project_name,
    project_author,
    project_version,
    project_version_array,
    conditions,
    conditions_array,
    conditions_array_object,
    conditions_array_object_values
};

struct irr_json_stack_element
{
    constexpr irr_json_stack_element(irr_json_element_type type_) noexcept
      : type(type_)
      , element(0)
    {}

    constexpr irr_json_stack_element(irr_json_element_type type_,
                                     irr::ID element_) noexcept
      : type(type_)
      , element(element_)
    {}

    constexpr bool is_project() const noexcept
    {
        return type == irr_json_element_type::project;
    }

    constexpr bool is_project_name() const noexcept
    {
        return type == irr_json_element_type::project_name;
    }

    constexpr bool is_project_author() const noexcept
    {
        return type == irr_json_element_type::project_author;
    }

    constexpr bool is_project_version() const noexcept
    {
        return type == irr_json_element_type::project_version;
    }

    constexpr bool is_project_version_array() const noexcept
    {
        return type == irr_json_element_type::project_version_array;
    }

    constexpr bool is_conditions() const noexcept
    {
        return type == irr_json_element_type::conditions;
    }

    constexpr bool is_conditions_array() const noexcept
    {
        return type == irr_json_element_type::conditions_array;
    }

    constexpr bool is_conditions_array_object() const noexcept
    {
        return type == irr_json_element_type::conditions_array_object;
    }

    constexpr bool is_conditions_array_object_values() const noexcept
    {
        return type == irr_json_element_type::conditions_array_object_values;
    }

    irr_json_element_type type = irr_json_element_type::none;
    irr::ID element;
};

struct irr_json_stack
{
    std::vector<irr_json_stack_element> stack;

    irr_json_stack()
    {
        stack.reserve(1024);
    }

    bool empty() const noexcept
    {
        return stack.empty();
    }

    irr_json_stack_element top() noexcept
    {
        return stack.back();
    }

    template<typename... Args>
    void emplace(Args&&... args) noexcept
    {
        stack.emplace_back(std::forward<Args>(args)...);
    }

    irr_json_stack_element pop() noexcept
    {
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
        return true;
    }

    bool Int(int i)
    {
        std::cout << "Int(" << i << ")" << std::endl;
        return true;
    }

    bool Uint(unsigned u)
    {
        std::cout << "Uint(" << u << ")" << std::endl;

        if (stack.top().is_project_version_array()) {
            if (model.version_major < 0)
                model.version_major = static_cast<int>(u);
            else if (model.version_minor < 0)
                model.version_minor = static_cast<int>(u);
            else if (model.version_patch < 0)
                model.version_patch = static_cast<int>(u);
        }

        return true;
    }

    bool Int64(int64_t i)
    {
        std::cout << "Int64(" << i << ")" << std::endl;
        return true;
    }

    bool Uint64(uint64_t u)
    {
        std::cout << "Uint64(" << u << ")" << std::endl;
        return true;
    }

    bool Double(double d)
    {
        std::cout << "Double(" << d << ")" << std::endl;
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

        if (elem.type == irr_json_element_type::project_name) {
            auto& name = model.names.alloc();
            name.name = str;
            model.name = model.names.get_id(name);
            stack.pop();
        } else if (elem.type == irr_json_element_type::project_author) {
            auto& name = model.names.alloc();
            name.name = str;
            model.author = model.names.get_id(name);
            stack.pop();
        }

        return true;
    }

    bool StartObject()
    {
        std::cout << "StartObject()" << std::endl;

        if (stack.empty()) {
            stack.emplace(irr_json_element_type::project);
        } else if (stack.top().is_conditions_array()) {
            stack.emplace(irr_json_element_type::conditions_array_object);
        }

        return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy)
    {
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha
                  << copy << ")" << std::endl;

        if (stack.top().is_project()) {
            if (!strncmp(str, "name", length))
                stack.emplace(irr_json_element_type::project_name);
            else if (!strncmp(str, "author", length))
                stack.emplace(irr_json_element_type::project_author);
            else if (!strncmp(str, "version", length))
                stack.emplace(irr_json_element_type::project_version);
            else if (!strncmp(str, "conditions", length))
                stack.emplace(irr_json_element_type::conditions);
        } else if (stack.top().is_conditions_array_object()) {
            auto& condition = model.conditions.alloc();
            auto& name = model.names.alloc();
            name.name = str;
            condition.name = model.names.get_id(name);
            condition.values = -1;

            stack.emplace(
              irr_json_element_type::conditions_array_object_values);
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
            stack.emplace(irr_json_element_type::project_version_array);
            model.version_major = -1;
            model.version_minor = -1;
            model.version_patch = -1;
        } else if (stack.top().is_conditions()) {
            stack.emplace(irr_json_element_type::conditions_array);
        }

        return true;
    }

    bool EndArray(rapidjson::SizeType elementCount)
    {
        std::cout << "EndArray(" << elementCount << ")" << std::endl;

        if (stack.top().is_project_version_array()) {
            stack.pop(); // Pop project version array
            stack.pop(); // Pop project version
        }

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
