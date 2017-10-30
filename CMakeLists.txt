cmake_minimum_required(VERSION 3.9)

project(bicla)

add_library(bicla INTERFACE)
target_include_directories(bicla INTERFACE include)

# -------------------------------------------------------------------------
# tests
if(BICLA_BUILD_TESTS)
    enable_testing ()
	add_subdirectory(unit_tests)
endif()
