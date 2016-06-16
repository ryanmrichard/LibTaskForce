cmake_minimum_required(VERSION 2.8)
project(LibTaskForceCore LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LTF_ROOT_DIR}/cmake/Modules/")

list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/external/cereal")
find_package(MPI REQUIRED)
find_package(TBB REQUIRED)
find_package(cereal REQUIRED)

set(LIBTASKFORCE_INCLUDE_DIRS ${LTF_ROOT_DIR} 
                              ${MPI_CXX_INCLUDE_PATH} 
                              ${TBB_INCLUDE_DIRS} 
                              ${CEREAL_INCLUDE_DIR}
)
include_directories(${LIBTASKFORCE_INCLUDE_DIRS})
add_subdirectory(Distributed)
add_subdirectory(Threading)
add_subdirectory(Hybrid)
add_library(taskforce $<TARGET_OBJECTS:Threading> 
                      $<TARGET_OBJECTS:Distributed>
                      $<TARGET_OBJECTS:Hybrid>
)
set(LIBTASKFORCE_LIBRARIES taskforce ${TBB_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_subdirectory(Tests)

configure_file("libtaskforceConfig.cmake.in" "libtaskforceConfig.cmake" @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libtaskforceConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX})
