cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(irritator VERSION 0.1.0 LANGUAGES CXX C)

list(APPEND CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Use the GNU standard installation directories. See
# https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html
include(GNUInstallDirs)

find_package(glfw3 REQUIRED)

find_package(Threads REQUIRED)
add_library(threads INTERFACE IMPORTED)
set_property(TARGET threads PROPERTY
  INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

add_definitions(-DFMT_HEADER_ONLY)
include_directories(${CMAKE_SOURCE_DIR}/external/fmt/include)
include_directories(${CMAKE_SOURCE_DIR}/external/rapidjson/include)

enable_testing()
add_subdirectory(lib)
add_subdirectory(app)
