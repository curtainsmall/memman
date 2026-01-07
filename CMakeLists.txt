cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(memman)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source build is not allowed")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to `Release` because it is not specified")
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Choose the type of build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

add_library(memman INTERFACE "memman.h" "memman.hpp")
target_compile_features(memman INTERFACE c_std_99)
target_include_directories(memman INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(memman PROPERTIES FOLDER "memman")
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES "memman.h" "memman.hpp")

if(MEMMAN_BUILD_TESTS)
    add_subdirectory(tests)
endif()
