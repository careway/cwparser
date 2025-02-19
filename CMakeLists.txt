cmake_minimum_required(VERSION 3.14)
project(cwparser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()


add_library(cwparser INTERFACE)
add_library(cwparser::cwparser ALIAS cwparser)
target_include_directories(cwparser INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add tests subdirectory if testing is enabled
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Installation configuration
install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
                      PATTERN "*.hpp"
)

# Generate and install export configuration
include(CMakePackageConfigHelpers)

# Write version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cwparserConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

# Generate the config file that includes the exports
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cwparserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cwparserConfig.cmake"
    INSTALL_DESTINATION lib/cmake/cwparser
)

# Install the config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cwparserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cwparserConfigVersion.cmake"
    DESTINATION lib/cmake/cwparser
)

# Export the targets at creation time
install(TARGETS cwparser
    EXPORT cwparserTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Then install the export
install(EXPORT cwparserTargets
    FILE cwparserTargets.cmake
    NAMESPACE cwparser::
    DESTINATION lib/cmake/cwparser
)