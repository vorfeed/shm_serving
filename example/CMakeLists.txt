aux_source_directory(. example_SRCS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(example_lib ${example_SRCS})

target_link_libraries(
  example_lib serving_lib
)

add_executable(visitor visitor.cpp)
target_link_libraries(visitor example_lib)
