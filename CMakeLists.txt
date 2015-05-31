cmake_minimum_required(VERSION 2.8)
project(3Displayer)

if (NOT DEFINED CMAKE_ENABLE_C99)
  set(CMAKE_ENABLE_C99 "-std=gnu99")
endif(NOT DEFINED CMAKE_ENABLE_C99)
set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99} -Wall -g") 

enable_testing()
include_directories(include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(src)
add_subdirectory(bin)
add_subdirectory(test)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/3Displayer)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)