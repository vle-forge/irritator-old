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
        IDs top_id;
        bool show_window;
    };

    data_array<Path, IDs> hierarchy;     // Stores the list of path.
    multi_linker<IDs, IDs> nodes_linker; // Stores hierarchy.
    std::deque<IDs> stack;               // Cache to store ID
    data_array<item, IDs> packages;      // List of opened packages.

    void init(int package_capacity, int file_capacity);

    void open(const std::filesystem::path& path);
    void close(item& item);

    void show();
};

} // namespace irr

#endif
