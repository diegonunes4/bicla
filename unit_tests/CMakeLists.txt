project(bicla_unit_tests)

include(../build/conanbuildinfo.cmake)
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
