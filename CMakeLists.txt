cmake_minimum_required(VERSION 3.14)
project(config_parser_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add test executable
add_executable(config_parser_test tests/config_parser_test.cpp)

# Add include directories
target_include_directories(config_parser_test PRIVATE include)

# Add tests to CTest
add_test(
    NAME ConfigParserTests 
    COMMAND config_parser_test
)

# Set test properties to parse the output
set_tests_properties(ConfigParserTests PROPERTIES
    PASS_REGULAR_EXPRESSION "Test Summary:.*Failed: 0"
    FAIL_REGULAR_EXPRESSION "\\[FAIL\\]|\\[EXCEPTION\\]"
) 