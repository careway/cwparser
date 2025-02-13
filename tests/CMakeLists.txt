cmake_minimum_required(VERSION 3.14)
project(cwparser_tests)

# Add test executable
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/cwparser_test.cpp)

# Add include directories
target_link_libraries(${PROJECT_NAME} PUBLIC cwparser)

# Add tests to CTest
add_test(NAME BasicFileOperations 
         COMMAND ${PROJECT_NAME} basic_file_operations)

add_test(NAME IntegerAndBooleanParsing 
         COMMAND ${PROJECT_NAME} integer_and_boolean_parsing)

add_test(NAME VectorParsing 
         COMMAND ${PROJECT_NAME} vector_parsing)

add_test(NAME NestedSectionParsing 
         COMMAND ${PROJECT_NAME} nested_section_parsing)

add_test(NAME SpaceSeparatedTupleParsing 
         COMMAND ${PROJECT_NAME} space_separated_tuple_parsing)

add_test(NAME ComplexDataTypeParsing 
         COMMAND ${PROJECT_NAME} complex_data_type_parsing)

add_test(NAME InvalidKeyAccess 
         COMMAND ${PROJECT_NAME} invalid_key_access)

add_test(NAME NestedKeyTraversal 
         COMMAND ${PROJECT_NAME} nested_key_traversal)

add_test(NAME MalformedInputHandling 
         COMMAND ${PROJECT_NAME} malformed_input_handling)

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