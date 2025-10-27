cmake_minimum_required(VERSION 3.16)
project(cwparser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    enable_testing()
    option(BUILD_TESTING "Build tests" ON)
else()
    option(BUILD_TESTING "Build tests" OFF)
endif()

add_library(cwparser INTERFACE)
add_library(cwparser::cwparser ALIAS cwparser)
target_include_directories(cwparser INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add tests subdirectory if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Installation configuration
install(DIRECTORY include/
    DESTINATION include
    COMPONENT ${PROJECT_NAME}
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
    COMPONENT ${PROJECT_NAME}
)

# Export the targets at creation time
install(TARGETS cwparser
    EXPORT cwparserTargets
    COMPONENT ${PROJECT_NAME}
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
    COMPONENT ${PROJECT_NAME}


)


# ============================================================================
# Versioning Configuration
# ============================================================================

# Parse Version from GIT TAG 
include(cmake/GitVersion.cmake)
get_version_from_git()

# Generate files from version
add_custom_target( ${PROJECT_NAME}_version
 COMMAND ${CMAKE_COMMAND} -DPROJECT_NAME=${PROJECT_NAME} -DTEMPLATE_IN=${CMAKE_CURRENT_SOURCE_DIR}/cmake/template -DDOXYFILE_OUT=${CMAKE_CURRENT_SOURCE_DIR}/doc -DINCLUDE_OUT=${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/genVersionFiles.cmake
 BYPRODUCTS  ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/version.h ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
)

# # Library/Archive output directory
# set_target_properties(${PROJECT_NAME} PROPERTIES
#   VERSION ${PROJECT_VERSION}
#   SOVERSION ${PROJECT_VERSION_MAJOR}
# )



# ============================================================================
# Doxygen
# ============================================================================

# Generation of Doxygen documentation
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API        
documentation (requires Doxygen)"  ${DOXYGEN_FOUND})

if(${BUILD_DOCUMENTATION})

  add_custom_target(${PROJECT_NAME}_doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} -d preprocessor ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile > /dev/null 2>&1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/doc/html ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex
    DEPENDS ${PROJECT_NAME}_version
    VERBATIM
  )

endif()

# ============================================================================
# CPack Configuration
# ============================================================================

# --- Basic Package Metadata ---
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cwparser for parsing with nodes and atributes.")
set(CPACK_PACKAGE_VENDOR "cwparser")
set(CPACK_PACKAGE_CONTACT "ctjimenez24@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGING_INSTALL_PREFIX /usr/local)


# --- Component Setup ---
# This matches the COMPONENT arguments in install()
set(CPACK_COMPONENT_ALL ${PROJECT_NAME})
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
# set(CPACK_COMPONENT_${PROJECT_NAME}_DISPLAY_NAME "Development Files")
# set(CPACK_COMPONENT_${PROJECT_NAME}_DESCRIPTION "Headers and static libraries for developing applications with MyAwesomeLib.")
# --- Generator-Specific Settings ---

# For ZIP/TGZ (archive generators)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# For DEB generator (Debian/Ubuntu)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS ${CMAKE_BINARY_DIR}/lib)

# For RPM generator (Fedora/CentOS)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_LICENSE "LIMITED") # Use a standard license identifier (e.g., MIT, BSD, GPLv3)

# --- Include CPack at the end ---
include(CPack)
