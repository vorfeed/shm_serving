cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_COMPILER "/usr/local/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")

project(shm_serving)

add_definitions(
  -w
  -pthread -pipe
  -ggdb3 -O3
  -std=c++1y -D__GXX_EXPERIMENTAL_CXX11__
)

set(PROJ_ROOT ${PROJECT_SOURCE_DIR})

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

include_directories(
  ${PROJ_ROOT}
  ${Boost_INCLUDE_DIRS}
  ${Protobuf_INCLUDE_DIRS}
)

link_directories(
  /usr/local/lib
)

add_subdirectory(shm_serving)
add_subdirectory(example)
add_subdirectory(tools)
add_subdirectory(benchmark)
