// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "irritator.hpp"

#include <algorithm>

static constexpr std::filesystem::directory_options fs_iterator_flags =
  std::filesystem::directory_options::follow_directory_symlink |
  std::filesystem::directory_options::skip_permission_denied;

static constexpr char default_package_name[] = "package-name";

namespace irr {

static void
fill_filesystem_paths(const std::filesystem::path& current,
                      std::vector<std::filesystem::path>& files,
                      std::vector<std::filesystem::path>& dirs)
{
    namespace fs = std::filesystem;

    std::error_code ec;

    for (auto it = fs::directory_iterator(current, fs_iterator_flags, ec),
              et = fs::directory_iterator();
         it != et;
         ++it) {
        if (it->is_directory())
            dirs.emplace_back(it->path());
        else if (it->is_regular_file())
            files.emplace_back(it->path());
    }

    std::sort(files.begin(), files.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.filename() < rhs.filename();
    });

    std::sort(dirs.begin(), dirs.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.filename() < rhs.filename();
    });
}

void
FilesystemDialog::init(const std::filesystem::path& path,
                       const char* name_,
                       const char* description_)
{
    namespace fs = std::filesystem;

    std::error_code ec;

    current = fs::is_directory(path, ec) ? path : fs::current_path(ec);
    selected.clear();
    files.clear();
    dirs.clear();

    strcpy(input, default_package_name);
    fill_filesystem_paths(current, files, dirs);

    name = name_;
    description = description_;
}

void
FilesystemDialog::show()
{
    switch (status) {
    case filesystem_dialog_status::open_file:
        if (open_file_dialog()) {
            status = filesystem_dialog_status::close;
            if (!selected.empty())
                main_window->package_windows.open(selected);
        }
        break;
    case filesystem_dialog_status::open_directory:
        if (open_directory_dialog()) {
            status = filesystem_dialog_status::close;
            if (!selected.empty())
                main_window->package_windows.open(selected);
        }
        break;
    case filesystem_dialog_status::new_directory:
        if (new_directory_dialog()) {
            status = filesystem_dialog_status::close;
            if (!selected.empty())
                printf("New package %s.\n", selected.string().c_str());
        }
        break;
    default:
        break;
    }
}

void
FilesystemDialog::refresh(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    std::error_code ec;

    current = fs::is_directory(path, ec) ? path : fs::current_path(ec);
    selected.clear();
    files.clear();
    dirs.clear();

    fill_filesystem_paths(current, files, dirs);
}

bool
FilesystemDialog::open_file_dialog()
{
    auto next = std::filesystem::path{};
    auto res{ false };

    ImGui::SetNextWindowSize(ImVec2(550, 680), true);
    if (ImGui::BeginPopupModal(name, nullptr)) {
        bool path_click = false;
        for (auto it = current.begin(), et = current.end(); it != et; ++it) {
            if (it != current.begin())
                ImGui::SameLine();

            if (ImGui::Button(it->filename().string().c_str())) {
                selected.clear();
                next.clear();
                for (auto jt = current.begin(); jt != it; ++jt)
                    next /= *jt;
                next /= *it;
                path_click = true;
                break;
            }
        }

        ImGui::BeginChild("##FALL");

        if (ImGui::Selectable("..##FALL", (selected == ".."))) {
            if (next.empty()) {
                selected.clear();
                next = current.parent_path();
                path_click = true;
            }
        }

        for (auto it = dirs.begin(), et = dirs.end(); it != et; ++it) {
            buffer = "[Dir] ";
            buffer += it->filename().string().c_str();

            if (ImGui::Selectable(buffer.c_str(), (*it == selected))) {
                selected = *it;

                if (next.empty()) {
                    selected.clear();
                    next = *it;
                    path_click = true;
                }

                break;
            }
        }

        for (auto it = files.begin(), et = files.end(); it != et; ++it) {
            buffer = "[File] ";
            buffer += it->filename().string().c_str();

            if (ImGui::Selectable(buffer.c_str(), (*it == selected))) {
                selected = *it;
                break;
            }
        }

        ImGui::EndChild();

        if (path_click) {
            refresh(next);
            current = next;
        }

        ImGui::Text("File Name:##FALL");
        ImGui::SameLine();

        if (ImGui::Button("Cancel##FALL", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            res = true;
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Ok##FALL", ImVec2(180, 0))) {
            ImGui::CloseCurrentPopup();
            res = true;
        }

        ImGui::EndPopup();
    }

    return res;
}

bool
FilesystemDialog::open_directory_dialog()
{
    namespace fs = std::filesystem;
    auto ec = std::error_code{};
    auto next = fs::path{};
    auto res{ false };

    ImGui::SetNextWindowSize(ImVec2(550, 680), true);
    if (ImGui::BeginPopupModal(name, nullptr)) {
        bool path_click = false;

        for (auto it = current.begin(), et = current.end(); it != et; ++it) {
            if (it != current.begin())
                ImGui::SameLine();

            if (ImGui::Button(it->filename().string().c_str())) {
                selected.clear();
                next.clear();
                for (auto jt = current.begin(); jt != it; ++jt)
                    next /= *jt;
                next /= *it;
                path_click = true;
                break;
            }
        }

        ImVec2 size = ImGui::GetContentRegionMax();
        size.y -= 120;
        ImGui::BeginChild("##FDir", size);

        if (ImGui::Selectable("..", false)) {
            if (next.empty()) {
                next = current.parent_path();
                path_click = true;
            }
        }

        if (ImGui::Selectable(".", false)) {
            if (next.empty()) {
                next = current;
                path_click = true;
            }
        }

        for (auto it = dirs.begin(), et = dirs.end(); it != et; ++it) {
            buffer = "[Dir] ";
            buffer += it->filename();

            if (ImGui::Selectable(buffer.c_str(), (*it == selected))) {
                selected = *it;

                if (next.empty()) {
                    selected.clear();
                    next = *it;
                    path_click = true;
                }

                break;
            }
        }

        ImGui::EndChild();

        if (path_click) {
            refresh(next);
            current = next;
        }

        bool show_ok_button = false;
        float width = ImGui::GetContentRegionAvailWidth();
        if (!current.empty()) {
            auto cmake_exists = current;
            auto descr_exists = current;

            cmake_exists /= "CMakeLists.txt";
            descr_exists /= "Description.txt";

            if (fs::is_regular_file(cmake_exists, ec) &&
                fs::is_regular_file(descr_exists, ec)) {
                show_ok_button = true;
                width /= 2.0f;

                if (ImGui::Button("Ok", ImVec2(width, 0))) {
                    selected = current;
                    ImGui::CloseCurrentPopup();
                    res = true;
                }
            }
        }

        ImGui::SetItemDefaultFocus();

        if (show_ok_button)
            ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(width, 0))) {
            selected.clear();
            ImGui::CloseCurrentPopup();
            res = true;
        }

        ImGui::EndPopup();
    }

    return res;
}

bool
FilesystemDialog::new_directory_dialog()
{
    namespace fs = std::filesystem;
    auto ec = std::error_code{};
    auto next = fs::path{};
    auto res{ false };

    ImGui::SetNextWindowSize(ImVec2(550, 680), true);
    if (ImGui::BeginPopupModal(name, nullptr)) {
        bool path_click = false;
        for (auto it = current.begin(), et = current.end(); it != et; ++it) {
            if (it != current.begin())
                ImGui::SameLine();

            if (ImGui::Button(it->c_str())) {
                selected.clear();
                next.clear();
                for (auto jt = current.begin(); jt != it; ++jt)
                    next /= *jt;
                next /= *it;
                path_click = true;
                break;
            }
        }

        ImVec2 size = ImGui::GetContentRegionMax();
        size.y -= 120;
        ImGui::BeginChild("##FDir", size);

        if (ImGui::Selectable("..", false)) {
            if (next.empty()) {
                next = current.parent_path();
                path_click = true;
            }
        }

        if (ImGui::Selectable(".", false)) {
            if (next.empty()) {
                next = current;
                path_click = true;
            }
        }

        for (auto it = dirs.begin(), et = dirs.end(); it != et; ++it) {
            buffer = "[Dir] ";
            buffer += it->filename();

            if (ImGui::Selectable(buffer.c_str(), (*it == selected))) {
                selected = *it;

                if (next.empty()) {
                    selected.clear();
                    next = *it;
                    path_click = true;
                }

                break;
            }
        }

        ImGui::EndChild();

        if (path_click) {
            refresh(next);
            current = next;
        }

        ImGui::InputText("Package", input, MAX_FILE_DIALOG_NAME_BUFFER);

        bool show_ok_button = false;
        float width = ImGui::GetContentRegionAvailWidth();
        if (strlen(input) > 0) {
            auto new_directory = current;

            if (!selected.empty())
                new_directory /= selected;

            new_directory /= input;

            if (!fs::is_directory(new_directory, ec)) {
                show_ok_button = true;
                width /= 2.0f;

                if (ImGui::Button("Ok", ImVec2(width, 0))) {
                    selected = new_directory;
                    ImGui::CloseCurrentPopup();
                    res = true;
                }
            }
        }

        ImGui::SetItemDefaultFocus();

        if (show_ok_button)
            ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(width, 0))) {
            selected.clear();
            ImGui::CloseCurrentPopup();
            res = true;
        }

        ImGui::EndPopup();
    }

    return res;
}

} // namespace irr
