cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(libirritator LANGUAGES CXX)

option(WITH_FULL_OPTIMIZATION "Disable all logging facilities and active heavy optimization code. [default: off]" OFF)
option(WITH_DEBUG "enable maximium debug code. [default: ON]" ON)
option(WITH_LOG "enable log message. [default: ON]" ON)

set(public_irritator_header
 include/irritator/data-array.hpp
 include/irritator/linker.hpp)

set(private_irritator_source
  src/private.cpp
  src/private.hpp
  src/simulation.cpp
  )

add_library(libirritator ${public_irritator_header} ${private_irritator_source})

option(BUILD_SHARED_LIBS "Build shared library" OFF)
include(GenerateExportHeader)
generate_export_header(libirritator
    EXPORT_MACRO_NAME BX_EXPORT
    NO_EXPORT_MACRO_NAME BX_NO_EXPORT
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/irritator/export)

target_compile_options(libirritator
  PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -pipe -march=native
      $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
  $<$<CXX_COMPILER_ID:MSVC>:
      $<$<CONFIG:Debug>:/Od /W3 /Zi>>
  $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)

target_include_directories(libirritator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>)

target_link_libraries(libirritator
  PRIVATE
  threads)

set_target_properties(libirritator PROPERTIES
  OUTPUT_NAME "irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  POSITION_INDEPENDENT_CODE ON
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

target_compile_definitions(libirritator
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:IRRITATOR_FULL_OPTIMIZATION>
  $<$<BOOL:${WITH_DEBUG}>:IRRITATOR_ENABLE_DEBUG>
  $<$<BOOL:${WITH_LOG}>:IRRITATOR_ENABLE_LOG>
  $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
  $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
  VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  VERSION_MINOR=${PROJECT_VERSION_MINOR}
  VERSION_PATCH=${PROJECT_VERSION_PATCH})

install(TARGETS libirritator
  EXPORT libirritator-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${LIBLEGACY_INCLUDE_DIRS})

configure_file(irritator.pc.in
  "${CMAKE_BINARY_DIR}/irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.pc"
  @ONLY)

install(FILES
  ${CMAKE_BINARY_DIR}/irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(DIRECTORY include
  DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/irritator/export
  DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/irritator-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

install(EXPORT libirritator-targets
    FILE libirritator-targets.cmake
    NAMESPACE libirritator::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/irritator-${VLE_ABI})

# This makes the project importable from the build directory
export(TARGETS libirritator FILE LibirritatorConfig.cmake)

function(irritator_add_test test_name)
  add_executable(${test_name} ${ARGN})

  set_target_properties(${test_name} PROPERTIES
    COMPILE_DEFINITIONS EXAMPLES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/test\"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

  target_compile_definitions(${test_name}
    PRIVATE
    $<$<BOOL:${WITH_DEBUG}>:IRRITATOR_ENABLE_DEBUG>
    $<$<BOOL:${WITH_LOG}>:IRRITATOR_ENABLE_LOG>
    $<$<BOOL:${NLOPT_FOUND}>:IRRITATOR_HAVE_NLOPT>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>)

  target_include_directories(${test_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR})

  target_link_libraries(${test_name} libirritator threads)

  add_test(${test_name} ${test_name})
endfunction()

if (NOT BUILD_SHARED_LIBS)
  irritator_add_test(test-container test/container.cpp)
endif ()
