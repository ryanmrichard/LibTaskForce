cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/external/madness")
list(APPEND CMAKE_PREFIX_PATH "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/external/cereal")
find_package(MPI REQUIRED)
find_package(MADNESS REQUIRED COMPONENTS world)
include_directories(include/LTFcereal/include)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)

configure_file("libtaskforceConfig.cmake.in" "libtaskforceConfig.cmake" @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libtaskforceConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX})
