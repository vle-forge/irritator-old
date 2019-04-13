// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "package.hpp"
#include "irritator.hpp"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui.h"

namespace irr {

static constexpr std::filesystem::directory_options fs_iterator_flags =
  std::filesystem::directory_options::follow_directory_symlink |
  std::filesystem::directory_options::skip_permission_denied;

static constexpr ImGuiTreeNodeFlags file_node_flags =
  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
  ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

void
Package::init(int package_capacity, int file_capacity)
{
    hierarchy.init(file_capacity);
    nodes_linker.init(file_capacity);
    packages.init(package_capacity);
}

void
Package::open(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    std::error_code ec;

    if (!fs::is_directory(path, ec)) {
        main_window->log_window.log(4, "Package: %s must be a directory.\n");
        return;
    }

    stack.clear();

    Path& top = hierarchy.alloc();
    IDs top_id = hierarchy.get_id(top);
    top.node = path;
    top.have_children = true;
    top.show_open = true;

    auto& new_package = packages.alloc();
    new_package.top_id = top_id;
    new_package.show_window = true;

    if (top.have_children)
        stack.emplace_back(top_id);

    while (!stack.empty()) {
        auto current_id = stack.front();
        stack.pop_front();

        auto* current_path = hierarchy.try_to_get(current_id);
        if (!current_path)
            continue;

        for (auto it = fs::directory_iterator(
               current_path->node, fs_iterator_flags, ec);
             it != fs::directory_iterator();
             ++it) {
            if (!ec && it->is_regular_file()) {
                auto& child = hierarchy.alloc();
                child.node = *it;
                child.have_children = false;
                child.show_open = false;
                child.is_read = false;
                nodes_linker.emplace(current_id, hierarchy.get_id(child));
            }
        }

        for (auto it = fs::directory_iterator(
               current_path->node, fs_iterator_flags, ec);
             it != fs::directory_iterator();
             ++it) {
            if (!ec && it->is_directory()) {
                auto& child = hierarchy.alloc();
                child.node = *it;
                child.have_children = true;
                child.show_open = false;
                child.is_read = false;
                nodes_linker.emplace(current_id, hierarchy.get_id(child));
                stack.push_back(hierarchy.get_id(child));
            }
        }
    }
}

void
Package::show()
{
    namespace fs = std::filesystem;

    item* ptr = nullptr;
    while (packages.next(ptr)) {
        auto* top_node = hierarchy.try_to_get(ptr->top_id);
        if (!top_node) {
            main_window->log_window.log(4, "Package: strange behavior\n");
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(350, 500), true);
        if (!ImGui::Begin(top_node->node.c_str(), &ptr->show_window)) {
            ImGui::End();
            return;
        }

        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f),
                           "%s",
                           top_node->node.filename().string().c_str());

        stack.clear();
        stack.push_back(ptr->top_id);

        while (!stack.empty()) {
            auto top_id = stack.front();
            auto* top = hierarchy.try_to_get(top_id);

            if (top->is_read) {
                stack.pop_front();

                if (top->show_open) {
                    for (auto& node :
                         nodes_linker.get_view(top_id, hierarchy)) {
                        if (!node.have_children) {
                            ImGui::TreeNodeEx(
                              node.node.filename().string().c_str(),
                              file_node_flags);
                            if (ImGui::IsItemClicked()) {
                                if (node.node.extension().string() == ".vpz") {
                                    printf("show vpz file\n");
                                    // auto& vpz =
                                    // gv.vpz_files[f.path.string()]; vpz.id =
                                    //   std::string("vpz-") +
                                    //   std::to_string(id_generator++);
                                    // vpz.open(f.path.string());
                                } else {
                                    printf("show txt file\n");
                                    // gv.txt_files.emplace(f.path.string(),
                                    //                      vle::glvle::Gltxt(f.path.string()));
                                }
                            }
                        }
                    }

                    ImGui::TreePop();
                }
            } else {
                top->is_read = true;

                if (ImGui::TreeNodeEx(top->node.filename().string().c_str())) {
                    top->show_open = true;

                    for (auto& sub_node :
                         nodes_linker.get_view(top_id, hierarchy)) {
                        if (sub_node.have_children)
                            stack.push_back(hierarchy.get_id(sub_node));
                    }
                } else {
                    top->show_open = false;
                }
            }
        }

        const float width = ImGui::GetContentRegionAvailWidth() / 4.f -
                            2.f * ImGui::GetStyle().FramePadding.x;

        ImGui::Separator();

        // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        // ImGui::Text("Binary: %s", package->getDir().c_str());
        // ImGui::Text("Source: %s",
        // package->getDir(vle::utils::PKG_SOURCE).c_str());
        // ImGui::PopStyleVar();

        // if (pst == package_status::none) {
        //     if (ImGui::Button("configure", ImVec2(width, 0))) {
        //         pst = package_status::configure;
        //         gv.log_w.log(
        //           6, "configure package %s\n",
        //           current.path.string().c_str());

        //         try {
        //             future_package_return =
        //               std::async(std::launch::async,
        //                          [](vle::utils::Package& p,
        //                             std::ostream& out,
        //                             std::ostream& err) -> bool {
        //                              p.configure();
        //                              p.wait(out, err);
        //                              return p.isSuccess();
        //                          },
        //                          std::ref(*package),
        //                          std::ref(log_package_out),
        //                          std::ref(log_package_err));

        //         } catch (const std::exception& /*e*/) {
        //             pst = package_status::none;
        //         }
        //     }
        //     ImGui::SameLine();
        //     if (ImGui::Button("build", ImVec2(width, 0))) {
        //         pst = package_status::build;

        //         gv.log_w.log(
        //           6, "build package %s\n", current.path.string().c_str());
        //         try {
        //             future_package_return =
        //               std::async(std::launch::async,
        //                          [](vle::utils::Package& p,
        //                             std::ostream& out,
        //                             std::ostream& err) -> bool {
        //                              p.build();
        //                              p.wait(out, err);
        //                              if (p.isSuccess()) {
        //                                  p.install();
        //                                  p.wait(out, err);
        //                                  return p.isSuccess();
        //                              }

        //                              return false;
        //                          },
        //                          std::ref(*package),
        //                          std::ref(log_package_out),
        //                          std::ref(log_package_err));

        //         } catch (const std::exception& /*e*/) {
        //             pst = package_status::none;
        //         }
        //     }
        //     ImGui::SameLine();
        //     if (ImGui::Button("clean", ImVec2(width, 0))) {
        //         pst = package_status::clean;
        //         gv.log_w.log(
        //           6, "clean package %s\n", current.path.string().c_str());
        //         try {
        //             future_package_return =
        //               std::async(std::launch::async,
        //                          [](vle::utils::Package& p) -> bool {
        //                              p.clean();
        //                              p.rclean();
        //                              return true;
        //                          },
        //                          std::ref(*package));

        //         } catch (const std::exception& /*e*/) {
        //             pst = package_status::none;
        //         }
        //     }
        //     ImGui::SameLine();
        //     if (ImGui::Button("test", ImVec2(width, 0))) {
        //         pst = package_status::test;
        //         gv.log_w.log(
        //           6, "test package %s\n", current.path.string().c_str());
        //         try {
        //             future_package_return =
        //               std::async(std::launch::async,
        //                          [](vle::utils::Package& p,
        //                             std::ostream& out,
        //                             std::ostream& err) -> bool {
        //                              p.test();
        //                              p.wait(out, err);
        //                              return p.isSuccess();
        //                          },
        //                          std::ref(*package),
        //                          std::ref(log_package_out),
        //                          std::ref(log_package_err));

        //         } catch (const std::exception& /*e*/) {
        //             pst = package_status::none;
        //         }
        //     }
        // } else {
        //     static const char* str_status[] = { "Nothing to do"
        //                                         "Doing configure...",
        //                                         "Doing build...",
        //                                         "Doing clean...",
        //                                         "Doing test..." };

        //     if (future_package_return.valid()) {
        //         {
        //             std::lock_guard<std::mutex> guard(log_package_return);
        //             gv.log_w.log(6, "%s", log_package_out.str().c_str());
        //             log_package_out.str(std::string());
        //             gv.log_w.log(5, "%s", log_package_err.str().c_str());
        //             log_package_err.str(std::string());
        //         }

        //         auto s =
        //         future_package_return.wait_for(std::chrono::seconds(0)); if
        //         (s
        //         == std::future_status::ready)
        //             pst = package_status::none;
        //         else
        //             ImGui::Text("%s", str_status[static_cast<int>(pst)]);
        //     } else
        //         ImGui::Text("%s", str_status[static_cast<int>(pst)]);
        // }

        ImGui::End();
    }
}

void
Package::close(item& item)
{
    packages.free(item);
}

} // namespace irr
