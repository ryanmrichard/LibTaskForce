set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(ThreadTest ThreadTest.cpp)
add_executable(DistTest DistTest.cpp)
add_executable(HybridTest HybridTest.cpp)

target_link_libraries(ThreadTest ${LIBTASKFORCE_LIBRARIES})    
target_link_libraries(DistTest ${LIBTASKFORCE_LIBRARIES})
target_link_libraries(HybridTest ${LIBTASKFORCE_LIBRARIES})
add_dependencies(ThreadTest taskforce)
add_dependencies(DistTest taskforce)
add_dependencies(HybridTest taskforce)
install(TARGETS ThreadTest RUNTIME DESTINATION bin)
install(TARGETS DistTest RUNTIME DESTINATION bin)
install(TARGETS HybridTest RUNTIME DESTINATION bin)