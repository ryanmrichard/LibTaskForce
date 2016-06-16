#####This is the top-level CMake script for LibTaskForce
#####The official name of the project is LibTaskForce, but
#####at select points throughout these CMakeLists.txt we suppress
#####the "lib" so that we don't get libraries like liblibtaskforce

#At this point we require:
# 1) C++11
# 2) MPI
# 3) Intel Thread Building Blocks (TBB)
# 4) Cereal (I'll grab it for you if you don't have it already)
#
#
# The following are some very useful CMake variables you should consider setting
#
# CMAKE_CXX_COMPILER  The C++ compiler
# MPI_CXX_COMPILER    The MPI wrapper compiler that goes with CMAKE_CXX_COMPILER
# CMAKE_CXX_FLAGS     Flags to pass to the C++ compiler
# CMAKE_INSTALL_PREFIX The path where things will be installed
# CMAKE_PREFIX_PATH  Where CMake will look for includes,binaries,libraries when
#                    find_package() is invoked
# CMAKE_BUILD_TYPE   Do you want debug symbols?
# CMAKE_BINARY_DIR   Where we are building (can also be specified with -B flag)

cmake_minimum_required(VERSION 3.0)
project(LibTaskForce LANGUAGES CXX)
include(ExternalProject)

#Add our special find cmake macros
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(MPI REQUIRED)
find_package(TBB REQUIRED)

#Sets some extra debug flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER) 
if(BUILD_TYPE_LOWER STREQUAL "debug")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wfloat-equal -Wshadow -Wconversion")
endif()

#This folder
set(LTF_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/stage${CMAKE_INSTALL_PREFIX}/external)
ExternalProject_Add(libtaskforce_core
                    SOURCE_DIR ${LTF_ROOT_DIR}/LibTaskForce
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                               -DDESTDIR=${CMAKE_BINARY_DIR}/stage
                               -DMPI_CXX_COMPILER=${MPI_CXX_COMPILER}
                               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                               -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"
                               -DLTF_ROOT_DIR=${LTF_ROOT_DIR}
                    BUILD_ALWAYS 1
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
)
add_dependencies(libtaskforce_core cereal_external)

install(DIRECTORY ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
