#####This is the top-level CMake script for LibTaskForce
#####The official name of the project is LibTaskForce, but
#####at select points throughout these CMakeLists.txt we suppress
#####the "lib" so that we don't get libraries like liblibtaskforce

cmake_minimum_required(VERSION 2.8)
project(LibTaskForce LANGUAGES C CXX)
include(ExternalProject)

#This whole thing is based around MPI so make sure we  have it from the getgo
find_package(MPI REQUIRED)

#We require C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wfloat-equal -Wshadow -Wswitch-default -Wconversion")
endif()

add_subdirectory(external)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/external)
ExternalProject_Add(libtaskforce_core
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtaskforce
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                               -DDESTDIR=${CMAKE_BINARY_DIR}/stage
                               -DMPI_CXX_COMPILER=${MPI_CXX_COMPILER}
                               -DMPI_C_COMPILER=${MPI_C_COMPILER}
                               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                               -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                               -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
)
add_dependencies(libtaskforce_core madness_external)
install(DIRECTORY ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
