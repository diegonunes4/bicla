project(bicla_unit_tests)

# -------------------------------------------------------------------------
# conan

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE ../conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)
# -------------------------------------------------------------------------

message(STATUS ****************** ${CMAKE_BINARY_DIR})
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_define_targets()
include(../config/cmake/ParseAndAddCatchTests.cmake)


set(bicla_unit_tests_source_files main.cpp parse_arguments.cpp)

add_executable(bicla_unit_tests ${bicla_unit_tests_source_files})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${bicla_unit_tests_source_files})

target_link_libraries(bicla_unit_tests
    bicla
	CONAN_PKG::Catch
)

set_target_properties(bicla_unit_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    )

target_compile_options(bicla_unit_tests PRIVATE 
    /W4 /WX /std:c++17 /permissive-
)

enable_testing()
ParseAndAddCatchTests(${PROJECT_NAME})
