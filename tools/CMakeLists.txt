aux_source_directory(. tools_SRCS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(tools_lib ${tools_SRCS})

target_link_libraries(
  tools_lib serving_lib
)

add_executable(shared_memory_remover shared_memory_remover.cpp)
target_link_libraries(shared_memory_remover tools_lib)
