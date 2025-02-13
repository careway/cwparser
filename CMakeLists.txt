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
add_test(NAME BasicFileOperations 
         COMMAND config_parser_test basic_file_operations)

add_test(NAME IntegerAndBooleanParsing 
         COMMAND config_parser_test integer_and_boolean_parsing)

add_test(NAME VectorParsing 
         COMMAND config_parser_test vector_parsing)

add_test(NAME NestedSectionParsing 
         COMMAND config_parser_test nested_section_parsing)

add_test(NAME SpaceSeparatedTupleParsing 
         COMMAND config_parser_test space_separated_tuple_parsing)

add_test(NAME ComplexDataTypeParsing 
         COMMAND config_parser_test complex_data_type_parsing)

add_test(NAME InvalidKeyAccess 
         COMMAND config_parser_test invalid_key_access)

add_test(NAME NestedKeyTraversal 
         COMMAND config_parser_test nested_key_traversal)

add_test(NAME MalformedInputHandling 
         COMMAND config_parser_test malformed_input_handling)

# Set test properties
set_tests_properties(BasicFileOperations 
                    IntegerAndBooleanParsing 
                    VectorParsing 
                    NestedSectionParsing 
                    SpaceSeparatedTupleParsing 
                    ComplexDataTypeParsing 
                    InvalidKeyAccess 
                    NestedKeyTraversal 
                    MalformedInputHandling
    PROPERTIES
    ENVIRONMENT "TEST_CONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/tests"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)