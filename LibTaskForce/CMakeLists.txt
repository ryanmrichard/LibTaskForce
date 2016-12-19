cmake_minimum_required(VERSION 2.8)
project(LibTaskForceCore LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LIBTASKFORCE_ROOT_DIR}/cmake/Modules/")

list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}")
find_package(MPI REQUIRED)
find_package(TBB REQUIRED)
find_package(cereal REQUIRED)

set(LIBTASKFORCE_INCLUDE_DIRS ${LIBTASKFORCE_ROOT_DIR}
                              ${MPI_CXX_INCLUDE_PATH}
                              ${TBB_INCLUDE_DIRS}
)
include_directories(${LIBTASKFORCE_INCLUDE_DIRS})

add_library(taskforce Distributed/ProcessComm.cpp
                      Distributed/ProcessEnv.cpp
                      Distributed/ProcessQueue.cpp
                      Distributed/Scheduler.cpp
                      Hybrid/HybridComm.cpp
                      Hybrid/HybridEnv.cpp
                      Threading/ThreadComm.cpp
                      Threading/ThreadEnv.cpp
           )

target_link_libraries(taskforce cereal)

set(LIBTASKFORCE_LIBRARIES taskforce ${TBB_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_subdirectory(Tests)
configure_file("libtaskforceConfig.cmake.in" "libtaskforceConfig.cmake" @ONLY)
install(TARGETS taskforce DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CMAKE_BINARY_DIR}/libtaskforceConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/libtaskforce)
