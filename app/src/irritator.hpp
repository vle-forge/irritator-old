// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_IRRITATOR_HPP
#define ORG_VLEPROJECT_IRRITATOR_IRRITATOR_HPP

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "package.hpp"

namespace irr {

struct irritator;

struct LogWindow
{
    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> line_offsets;

    bool auto_scroll = true;
    bool scroll_to_bottom = false;
    bool show_window = true;

    LogWindow() = default;
    void clear() noexcept;

    void log(int level, const char* fmt, ...) IM_FMTARGS(3);
    void log(int level, const char* fmt, va_list args) IM_FMTLIST(3);
    void show();
};

struct irritator
{
    Package package_windows;
    LogWindow log_window;

    int init(int argc, char* argv[]);

    void show();
};

} // irr

#endif
