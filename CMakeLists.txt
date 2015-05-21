cmake_minimum_required(VERSION 2.8)
project(3Displayer)

include_directories(include)
add_subdirectory(src)
add_subdirectory(bin)

set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99} -Wall")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/3Displayer)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)