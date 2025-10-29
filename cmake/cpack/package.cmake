function(parse_cache_variable_value file_path variable_name output_variable)
    if(NOT EXISTS ${file_path})
        message(WARNING "File not found, cannot parse variable '${variable_name}': ${file_path}")
        set(${output_variable} "" PARENT_SCOPE)
        return()
    endif()

    # Read the entire file content into a variable
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19")
        file(REAL_PATH "${file_path}" file_rp)
    else()
        get_filename_component(file_rp "${file_path}" ABSOLUTE)
    endif()

    file(STRINGS ${file_rp} file_content REGEX "^${variable_name}:[A-Z]+=(.*)$")


    # Use a regular expression to find the line and capture the value.
    # This regex looks for:
    #   ^                  - Start of a line
    #   ${variable_name}   - The variable name you passed in
    #   :[A-Z]+            - A colon, followed by one or more uppercase letters (the type, e.g., STRING, BOOL)
    #   =                  - An equals sign
    #   (.*)               - A capturing group for the value (any character, any number of times)
    #   $                  - End of the line
    string(REGEX MATCH "^${variable_name}:[A-Z]+=(.*)$" match_result "${file_content}")

    if(match_result)
        # The captured value is in the special variable CMAKE_MATCH_1
        set(extracted_value "${CMAKE_MATCH_1}")
        message(STATUS "Found '${variable_name}' in ${file_path} with value: '${extracted_value}'")
        set(${output_variable} "${extracted_value}" PARENT_SCOPE)
    else()
        message(STATUS "Could not find variable '${variable_name}' in ${file_path}")
        set(${output_variable} "" PARENT_SCOPE)
    endif()
endfunction()


parse_cache_variable_value("./shared/CMakeCache.txt" CMAKE_BUILD_TYPE build_type_from_cache)

if(build_type_from_cache STREQUAL "Debug")
    include("shared/CPackSourceConfig.cmake")
else()
    include("shared/CPackConfig.cmake")
endif()
set(CPACK_INSTALL_CMAKE_PROJECTS
    shared cwparser ALL /
    static cwparser ALL /
)