// Copyright (c) 2019 INRA Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "irritator.hpp"

#include <cstring>

namespace irr {

enum class main_error_type
{
    success,
    help,
    short_argument_error,
    long_argument_error,
    open_package_error
};

struct command_line_status
{
    command_line_status(main_error_type status_)
      : status(status_)
    {}

    command_line_status(char* package_, int optind_)
      : package(package_)
      , optind(optind_)
    {}

    char* package = nullptr;
    int optind = -1;
    main_error_type status = main_error_type::success;
};

// Parse command line option
// irritator [options...] [vpz files...]
// - [--help][-h]           show help message.
// - `-P[ ]str`             short option to open a package.
// - --package[:|=]str]     long option to open a package.
// - [vpz files...]         list of vpz file to open after in the package.
//
// Use the package option to open a package directly when starting irritator.
static command_line_status
parse_argument(int argc, char* argv[])
{
    const auto* short_package = "-P";
    const auto* long_package = "--package";
    const auto short_package_sz = std::strlen(short_package);
    const auto long_package_sz = std::strlen(long_package);
    char* package = nullptr;
    int optind = argc;
    int i = 1;

    while (i < optind) {
        const auto len = strlen(argv[i]);

        if (!std::strncmp(argv[i], short_package, short_package_sz)) {
            if (len > short_package_sz) {
                package = argv[i] + short_package_sz;
            } else {
                if (i + 1 < argc) {
                    ++i;
                    package = argv[i];
                } else {
                    return command_line_status(
                      main_error_type::short_argument_error);
                }
            }
        } else if (!std::strncmp(argv[i], long_package, long_package_sz)) {
            if (len > long_package_sz && (argv[i][long_package_sz] == '=' ||
                                          argv[i][long_package_sz] == ':')) {
                package = argv[i] + long_package_sz;
            } else {
                if (i + 1 < argc) {
                    ++i;
                    package = argv[i];
                } else {
                    return command_line_status(
                      main_error_type::long_argument_error);
                }
            }
        } else if (!std::strcmp(argv[i], "-h") ||
                   !std::strcmp(argv[i], "--help")) {
            return command_line_status(main_error_type::help);
        } else {
            --optind;
            if (i < optind) {
                auto* ptr = argv[i];
                argv[i] = argv[optind];
                argv[optind] = ptr;
            }
        }

        ++i;
    }

    return command_line_status(package, optind);
}

int
irritator::init(int argc, char* argv[])
{
    package_windows.init(64, 1024);

    auto args = parse_argument(argc, argv);

    switch (args.status) {
    case main_error_type::success:
        if (args.package) {
            // Need to open package
        }
        break;
    case main_error_type::help:
        puts("glvle [options...] [vpz files...]\n\n"
             "Options:\n"
             " --help|-h              show this help message.\n"
             " -P[ ]str               short option to open a package.\n"
             " --package[:|=]str]     long option to open a package.\n"
             " [vpz files...]         list of vpz file to open after in the "
             "package.\n");
        return 0;
    case main_error_type::short_argument_error:
        fprintf(stderr, "Short argument error\n");
        return 1;
    case main_error_type::long_argument_error:
        fprintf(stderr, "Long argument error\n");
        return 2;
    case main_error_type::open_package_error:
        fprintf(stderr, "Fail to open package %s\n", args.package);
        return 3;
    };

        // For all other argument in command line (from the args.optind index),
        // search vpz files and try to open it in the experiment directory of
        // the package
#if 0
    if (args.package) {
        for (; args.optind != argc; ++args.optind) {
            auto dot = rindex(argv[args.optind], '.');

            if (dot) {
                if (strcmp(dot, ".vpz") == 0) {
                    auto p = vle::utils::Path(
                      gv.package.package->getDir(vle::utils::PKG_SOURCE));
                    p /= "exp";
                    p /= argv[args.optind];

                    if (p.exists()) {
                        try {
                            auto& vpz = gv.vpz_files[p.string()];
                            vpz.id = std::string("vpz-") +
                                     std::to_string(gv.package.id_generator++);
                            vpz.open(p.string());
                            printf("  file: %s open\n", p.string().c_str());
                        } catch (const std::exception& e) {
                            printf("  file: %s fail to open: %s\n",
                                   p.string().c_str(),
                                   e.what());
                            gv.vpz_files.erase(p.string());
                        }
                    } else {
                        fprintf(stderr,
                                "File %s does not exist\n",
                                p.string().c_str());
                    }
                } else {
                    fprintf(stderr,
                            "Can not open a non vpz file (%s)\n",
                            argv[args.optind]);
                }
            } else {
                fprintf(stderr,
                        "Can not open a non vpz file (%s)\n",
                        argv[args.optind]);
            }
        }
    }
#endif
    return 0;
}

static void
show_menubar(irritator& irr)
{
    bool new_box = false;
    bool open_box = false;
    std::string path_name;
    std::string dir_name;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // if (ImGui::MenuItem("New", "Ctrl+N"))
            //     new_box = true;
            // if (ImGui::MenuItem("Open", "Ctrl+O"))
            //     open_box = true;
            ImGui::MenuItem("New", "Ctrl+N");
            ImGui::MenuItem("Open", "Ctrl+O");
            ImGui::Separator();
            // if (ImGui::MenuItem("Close", nullptr, gv.have_package))
            //     gv.clear();

            ImGui::Separator();
            ImGui::MenuItem("Quit", "Ctrl+Q", false, false);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            Package::item* ptr = nullptr;
            while (irr.package_windows.packages.next(ptr)) {
                auto& top_node =
                  irr.package_windows.hierarchy.get(ptr->top_id);
                ImGui::MenuItem(
                  "Package Window##ID", nullptr, &ptr->show_window, true);
            }

            ImGui::MenuItem(
              "Log window", nullptr, &irr.log_window.show_window);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // if (new_box)
    //     ImGui::OpenPopup("New package");

    // if (open_box)
    //     ImGui::OpenPopup("Open package");

    // if (select_new_directory_dialog("New package",
    //                                 "Select a new directory",
    //                                 "/home/gquesnel/devel/bits",
    //                                 path_name,
    //                                 dir_name)) {
    //     if (!path_name.empty() && !dir_name.empty()) {
    //         try {
    //             gv.open(path_name, dir_name, true);
    //         } catch (const std::exception& e) {
    //             printf("%s.\n", e.what());
    //         }
    //     }
    // }

    // if (select_directory_dialog("Open package",
    //                             "Select a new directory",
    //                             "/home/gquesnel/devel/bits",
    //                             path_name,
    //                             dir_name)) {
    //     if (!dir_name.empty()) {
    //         try {
    //             gv.open(path_name, dir_name, false);
    //         } catch (const std::exception& e) {
    //             printf("%s.\n", e.what());
    //         }
    //     }
    // }
}

void
irritator::show()
{
    show_menubar(*this);

    package_windows.show();

    if (log_window.show_window)
        log_window.show();

    // if (gv.show_package_window)
    //     gv.show_package_window = gv.package.show(gv);

    // if (gv.show_log_window)
    //     gv.log_w.show(gv);

    // {
    //     auto it = gv.txt_files.begin();

    //     while (it != gv.txt_files.end()) {
    //         if (!it->second.show())
    //             it = gv.txt_files.erase(it);
    //         else
    //             ++it;
    //     }
    // }

    // {
    //     auto it = gv.vpz_files.begin();

    //     while (it != gv.vpz_files.end()) {
    //         if (!it->second.show())
    //             it = gv.vpz_files.erase(it);
    //         else
    //             ++it;
    //     }
    // }
}

} // namespace irr
