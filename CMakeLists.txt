project(simpleuv)
cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(libuv REQUIRED)
include_directories(${LIBUV_INCLUDE_DIR})
link_directories(${LIBUV_LIBRARY_DIR})

add_subdirectory(src)