set(main_sources
  src/main.cpp
  # src/glvle.hpp
  # src/glvle.cpp
  # src/menubar.cpp
  # src/log_window.cpp
  # src/dialog.cpp
  # src/package_window.cpp
  # src/text_window.cpp
  # src/vpz_window.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui.h
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui_demo.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/imgui_internal.h
  ${PROJECT_SOURCE_DIR}/external/imgui/imconfig.h
  ${PROJECT_SOURCE_DIR}/external/imgui/imstb_rectpack.h
  ${PROJECT_SOURCE_DIR}/external/imgui/imstb_textedit.h
  ${PROJECT_SOURCE_DIR}/external/imgui/imstb_truetype.h
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/imgui_impl_glfw.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/imgui_impl_glfw.h
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/imgui_impl_opengl3.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/imgui_impl_opengl3.h
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/libs/gl3w/GL/gl3w.c
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/libs/gl3w/GL/gl3w.h
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/libs/gl3w/GL/glcorearb.h)

add_executable(app ${main_sources})

target_include_directories(app
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/external/imgui
  ${PROJECT_SOURCE_DIR}/external/imgui/examples
  ${PROJECT_SOURCE_DIR}/external/imgui/examples/libs/gl3w)

target_link_libraries(app
  PRIVATE
  stdc++fs
  threads
  $<$<PLATFORM_ID:Linux>:dl>
  GL
  glfw
  libirritator
  ) # or c++fs

target_compile_definitions(app
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:IRRITATOR_FULL_OPTIMIZATION>
  $<$<BOOL:${WITH_DEBUG}>:IRRITATOR_ENABLE_DEBUG>
  $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
  $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
  VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  VERSION_MINOR=${PROJECT_VERSION_MINOR}
  VERSION_PATCH=${PROJECT_VERSION_PATCH})

set_target_properties(app PROPERTIES
  OUTPUT_NAME "irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)


install(TARGETS app
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
