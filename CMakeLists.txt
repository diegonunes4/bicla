cmake_minimum_required(VERSION 3.9)

project(bicla)

add_library(bicla INTERFACE)
target_include_directories(bicla INTERFACE include)

if(BICLA_BUILD_TESTS)
add_subdirectory(unit_tests)
endif()
