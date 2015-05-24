cmake_minimum_required(VERSION 2.8)
project(3Displayer)

if (NOT DEFINED CMAKE_ENABLE_C99)
  set(CMAKE_ENABLE_C99 "-std=gnu99")
endif(NOT DEFINED CMAKE_ENABLE_C99)
set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99} -Wall -g") 


include_directories(include)
add_subdirectory(src)
add_subdirectory(bin)


set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/3Displayer)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)