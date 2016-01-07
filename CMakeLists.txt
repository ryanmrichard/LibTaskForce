cmake_minimum_required(VERSION 2.8)
project(LibParallel CXX)
include(ExternalProject)

find_package(MPI REQUIRED)
message(STATUS ${MPI_CXX_LIBRARIES})
message(STATUS ${MPI_LINK_FLAGS})
set(INTEL_ROOT /theoryfs2/common/software/intel2015/composer_xe_2015.3.187)
set(INTEL_TBB ${INTEL_ROOT}/tbb/lib/intel64/gcc4.4/libtbb.so)
#We require C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ExternalProject_Add(madness
      PREFIX "${CMAKE_CURRENT_BINARY_DIR}/Madness"
      GIT_REPOSITORY https://github.com/m-a-d-n-e-s-s/madness
      GIT_TAG 327e406c9028087ebfb3b970f1bec0b8f70c7d1e
      UPDATE_COMMAND ""
      CMAKE_COMMAND ${CMAKE_COMMAND}
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
              -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
              -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
              -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
              -DBLAS_LIBRARIES=-mkl
              -DENABLE_UNITTESTS=off
              -DENABLE_LIBXC=off
              -DENABLE_ACML=off
              -DENABLE_GPERFTOOLS=off
              INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
)
set(MAD_LIBRARY_FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libMADworld.so
                      ${CMAKE_CURRENT_BINARY_DIR}/lib/libMADlinalg.so
                      ${CMAKE_CURRENT_BINARY_DIR}/lib/libMADmisc.so
                      ${CMAKE_CURRENT_BINARY_DIR}/lib/libMADtensor.so
)
set(PAR_SRC Communicator.hpp
            Communicator.cpp
            Environment.hpp
            Environment.cpp
            Future.hpp
)


add_executable(ParallelTest main.cpp ${PAR_SRC})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_LINK_FLAGS}")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include ${MPI_INCLUDE_PATH})
target_link_libraries(ParallelTest ${MAD_LIBRARY_FILES} ${INTEL_TBB} ${MPI_CXX_LIBRARIES})
add_dependencies(ParallelTest madness)
