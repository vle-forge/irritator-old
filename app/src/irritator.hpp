// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ORG_VLEPROJECT_IRRITATOR_IRRITATOR_HPP
#define ORG_VLEPROJECT_IRRITATOR_IRRITATOR_HPP

#include <filesystem>
#include <string>
#include <vector>

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

#define MAX_FILE_DIALOG_NAME_BUFFER 1024

struct FilesystemDialog
{
    std::filesystem::path selected;
    std::filesystem::path current;

    std::vector<std::filesystem::path> files;
    std::vector<std::filesystem::path> dirs;

    std::string buffer;

    char input[MAX_FILE_DIALOG_NAME_BUFFER];
    const char* name = nullptr;
    const char* description = nullptr;

    enum class filesystem_dialog_status
    {
        close,
        open_file,
        open_directory,
        new_directory
    };

    filesystem_dialog_status status = filesystem_dialog_status::close;

    void init(const std::filesystem::path& path,
              const char* name,
              const char* description);

    void refresh(const std::filesystem::path& path);

    void show();

private:
    bool open_file_dialog();
    bool open_directory_dialog();
    bool new_directory_dialog();
};

struct irritator
{
    Package package_windows;
    LogWindow log_window;
    FilesystemDialog filesystem_dialog;

    int init(int argc, char* argv[]);

    void show();

private:
    void show_menubar();
};

} // irr

#endif
