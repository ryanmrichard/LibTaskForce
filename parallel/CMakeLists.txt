cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/external/madness")
find_package(MPI REQUIRED)
find_package(MADNESS REQUIRED CONFIG COMPONENTS world)

add_subdirectory(src)
add_subdirectory(test)
set(PARALLEL_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include ${MADNESS_INCLUDE_DIRS})
set(PARALLEL_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib ${MADNESS_LIBRARIES})
