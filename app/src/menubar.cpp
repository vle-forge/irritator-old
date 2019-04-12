// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "irritator.hpp"

#include <cstdio>

namespace irr {

void
irritator::show_menubar()
{
    namespace fs = std::filesystem;

    auto ec = std::error_code{};
    auto new_box{ false };
    auto open_box{ false };

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                if (filesystem_dialog.status ==
                    FilesystemDialog::filesystem_dialog_status::close) {
                    filesystem_dialog.status = FilesystemDialog::
                      filesystem_dialog_status::new_directory;
                    filesystem_dialog.init(fs::current_path(),
                                           "New package",
                                           "Select a new directoy");
                }
                new_box = true;
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                if (filesystem_dialog.status ==
                    FilesystemDialog::filesystem_dialog_status::close) {
                    filesystem_dialog.status = FilesystemDialog::
                      filesystem_dialog_status::open_directory;
                    filesystem_dialog.init(
                      fs::current_path(), "Open package", "Select a directoy");
                }
                open_box = true;
            }
            ImGui::Separator();

            ImGui::Separator();
            ImGui::MenuItem("Quit", "Ctrl+Q", false, false);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            Package::item* ptr = nullptr;
            while (package_windows.packages.next(ptr)) {
                auto& top_node = package_windows.hierarchy.get(ptr->top_id);
                ImGui::MenuItem(
                  "Package Window##ID", nullptr, &ptr->show_window, true);
            }

            ImGui::MenuItem("Log window", nullptr, &log_window.show_window);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (new_box)
        ImGui::OpenPopup("New package");

    if (open_box)
        ImGui::OpenPopup("Open package");

    filesystem_dialog.show();
}

} // namespace irr
