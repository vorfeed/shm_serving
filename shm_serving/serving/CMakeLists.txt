add_subdirectory(config)
add_subdirectory(data_loader)
add_subdirectory(proto)
add_subdirectory(util)

aux_source_directory(. serving_SRCS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(serving_lib ${serving_SRCS})

target_link_libraries(
  serving_lib proto
  pthread rt
  protobuf glog gflags
  boost_thread boost_system boost_filesystem
)

add_executable(serving main.cpp)
target_link_libraries(serving serving_lib)
