include(ExternalProject)

# Find if it already exists?
find_package(cereal QUIET)                                

# If not found, build it
# (Actually, cereal doesn't have a make install, so skip building, etc,
# and just copy the headers
if(NOT CEREAL_FOUND)
  message(STATUS "We will install cereal for you")
  ExternalProject_Add(cereal_external
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cereal-source
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/external
    BUILD_COMMAND echo "Skipping building for header-only library"
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
  )
else()
  # Add a dummy library for dependencies
  add_library(cereal_external INTERFACE)
endif()
