aux_source_directory(. proto_SRCS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(config.pb.cc config.pb.h config.proto)
add_library(proto ${proto_SRCS} config.pb.cc config.pb.h)
