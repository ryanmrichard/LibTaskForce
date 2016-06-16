cmake_minimum_required (VERSION 2.6.2)
project (cereal)

option(SKIP_PORTABILITY_TEST "Skip portability tests" OFF)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cereal)
configure_file("cerealConfig.cmake.in" "cerealConfig.cmake" @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/cerealConfig.cmake 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cereal)
