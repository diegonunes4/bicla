cmake_minimum_required(VERSION 3.9)

project(bicla)

add_subdirectory(unit_tests)

#-------------------------------------------------------------------------------
add_library(bicla INTERFACE)
target_include_directories(bicla INTERFACE include)
