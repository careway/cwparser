include(${CMAKE_CURRENT_LIST_DIR}/GitVersion.cmake)
get_version_from_git()

configure_file(${TEMPLATE_IN}/Doxyfile.in ${DOXYFILE_OUT}/Doxyfile @ONLY)
configure_file(${TEMPLATE_IN}/version.h.in ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/version.h @ONLY)
