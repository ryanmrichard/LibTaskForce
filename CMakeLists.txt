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

cmake_minimum_required(VERSION 3.1.3)
project(LibTaskForce LANGUAGES CXX)
set(CMAKE_CXX_EXTENSIONS False CACHE BOOL "Enable/Disable compiler-specific C++ extensions")
set(CMAKE_CXX_STANDARD 14 CACHE STRING "Which C++ standard to use")

# CMake doesn't support Intel CXX standard until cmake 3.6
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  if("${CMAKE_VERSION}" VERSION_LESS "3.6")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")
  endif()
endif()

include(ExternalProject)

#Add our special find cmake macros
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(MPI REQUIRED)
find_package(TBB REQUIRED)


#Sets some extra debug flags
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER) 
if(BUILD_TYPE_LOWER STREQUAL "debug")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wfloat-equal -Wshadow -Wconversion")
endif()

#This folder
set(LIBTASKFORCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/stage${CMAKE_INSTALL_PREFIX}/external)
ExternalProject_Add(libtaskforce_core
   SOURCE_DIR ${LIBTASKFORCE_ROOT_DIR}/LibTaskForce
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DDESTDIR=${CMAKE_BINARY_DIR}/stage
              -DMPI_CXX_COMPILER=${MPI_CXX_COMPILER}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
              -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"
              -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
              -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
              -DLIBTASKFORCE_ROOT_DIR=${LIBTASKFORCE_ROOT_DIR}
   CMAKE_CACHE_ARGS -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
                    -DCMAKE_MODULE_PATH:STRING=${CMAKE_MODULE_PATH}
                    -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
                    -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
   BUILD_ALWAYS 1
   INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_BINARY_DIR}/stage
)
add_dependencies(libtaskforce_core cereal_external)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/LibTaskForce
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share USE_SOURCE_PERMISSIONS)
install(DIRECTORY ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)

include(CTest)
enable_testing()
set(TEST_BIN ${CMAKE_BINARY_DIR}/stage/${CMAKE_INSTALL_PREFIX}/bin)
set(THREAD_EXE ${TEST_BIN}/ThreadTest)
set(DIST_EXE ${TEST_BIN}/DistTest)
set(HYBRID_EXE ${TEST_BIN}/HybridTest)

#The mpirun's should really be ${MPIEXEC}, which is set by find mpi, but mpiexec
#uses Python 2 still....

add_test(NAME ThreadingTest 
         COMMAND ${THREAD_EXE} 2 30 500000000
)
add_test(NAME DistributedTest 
         COMMAND mpirun -n 2 ${DIST_EXE} 600 6
)
add_test(NAME HybridDistributed 
         COMMAND mpirun -n 2 ${HYBRID_EXE} 1 600 6
)
add_test(NAME HybridThreading
         COMMAND mpirun -n 1 ${HYBRID_EXE} 2 600 6
)
