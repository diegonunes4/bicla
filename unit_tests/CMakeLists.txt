project(bicla_unit_tests)

# -------------------------------------------------------------------------
# conan

# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
endif ()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE ../conanfile.txt
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

get_cmake_property(is_multi GENERATOR_IS_MULTI_CONFIG)

if (is_multi)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
else ()
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
endif ()

# -------------------------------------------------------------------------
macro(clear_default_msvc_cpp_properties)
    # Force to always compile with W4
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4] " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()
    if (CMAKE_CXX_FLAGS MATCHES "/WX- ")
        string(REGEX REPLACE "/WX-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()
endmacro()
# -------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    clear_default_msvc_cpp_properties()
endif ()

include(${CMAKE_BINARY_DIR}/unit_tests/conanbuildinfo.cmake)
include(../config/cmake/ParseAndAddCatchTests.cmake)


set(bicla_unit_tests_source_files main.cpp parse_arguments.cpp)

add_executable(bicla_unit_tests ${bicla_unit_tests_source_files})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${bicla_unit_tests_source_files})

target_link_libraries(bicla_unit_tests
        bicla
        CONAN_PKG::catch2
        )

set_target_properties(bicla_unit_tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(bicla_unit_tests PRIVATE
            /W4 /WX /std:c++17 /permissive-
            )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GCC")
    target_compile_options(bicla_unit_tests PRIVATE
            -Werror
            )
endif ()

enable_testing()
ParseAndAddCatchTests(${PROJECT_NAME})
