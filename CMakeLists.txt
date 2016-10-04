cmake_minimum_required(VERSION 2.8)
project(3Displayer C)

set(CMAKE_MODULE_PATH ${3Displayer_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -g -pg")

if (UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
  set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined")
endif(UNIX)

find_package(Sanitizers)
find_package(SDL2)
find_package(GLIB COMPONENTS gmodule gio gobject REQUIRED )

enable_testing()
include_directories(include lib_desparbes/include ${SDL2_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS})
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(lib_desparbes)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(test)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
