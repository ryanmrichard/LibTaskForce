cmake_minimum_required(VERSION 2.8)
project(LibParallel CXX)
enable_language(C)
include(ExternalProject)

find_package(MPI REQUIRED)

#We require C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   message(STATUS "Debug mode active")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

add_subdirectory(external)
ExternalProject_Add(taskforce_core
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/taskforce
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                               -DDESTDIR=${CMAKE_BINARY_DIR}/stage
                               -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/external
                               -DMPI_CXX_COMPILER=${MPI_CXX_COMPILER}
                               -DMPI_C_COMPILER=${MPI_C_COMPILER}
                               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                               -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
)
add_dependencies(taskforce_core madness_external)
install(DIRECTORY ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
