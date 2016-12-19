include(ExternalProject)

# Find if it already exists?
find_package(cereal QUIET)                                

# If not found, build it
# (Actually, cereal doesn't have a make install, so skip building, etc,
# and just copy the headers
if(NOT CEREAL_FOUND)
  message(STATUS "We will install cereal for you")
  ExternalProject_Add(cereal_external
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    GIT_TAG bd839ccb47dfb5462331fede7c5f013ca8dc60cb
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DJUST_INSTALL_CEREAL=True
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
  )
else()
  # Add a dummy library for dependencies
  add_library(cereal_external INTERFACE)
endif()
