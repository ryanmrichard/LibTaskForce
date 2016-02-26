cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/external/madness")
find_package(MPI REQUIRED)
find_package(MADNESS REQUIRED CONFIG COMPONENTS world)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)
configure_file("taskforceConfig.cmake.in" "taskforceConfig.cmake" @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/taskforceConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX})
