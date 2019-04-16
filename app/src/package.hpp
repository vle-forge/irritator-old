// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_PACKAGE_HPP
#define ORG_VLEPROJECT_IRRITATOR_PACKAGE_HPP

#include <deque>
#include <filesystem>

#include <irritator/data-array.hpp>
#include <irritator/linker.hpp>

namespace irr {

struct irritator;

struct Package
{
    struct Path
    {
        Path() noexcept = default;

        std::filesystem::path node;
        bool have_children = false;
        bool show_open = false;
        bool is_read = false;
    };

    struct item
    {
        ID top_id;
        bool show_window;
    };

    data_array<Path, ID> hierarchy;    // Stores the list of path.
    multi_linker<ID, ID> nodes_linker; // Stores hierarchy.
    std::deque<ID> stack;              // Cache to store ID
    data_array<item, ID> packages;     // List of opened packages.
    irritator* main_window = nullptr;

    void init(int package_capacity, int file_capacity);

    void open(const std::filesystem::path& path);
    void close(item& item);

    void show();
};

} // namespace irr

#endif
