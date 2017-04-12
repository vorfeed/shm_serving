// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <fcntl.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

namespace shm_serving {

template <class PB>
std::pair<bool, PB> ParseConfig(const std::string& config_path) {
  PB config;
  int32_t file_fd = open(config_path.c_str(), O_RDONLY);
  if(file_fd < 0) {
    return { false, config };
  }
  google::protobuf::io::FileInputStream infile(file_fd);
  infile.SetCloseOnDelete(true);
  if (!google::protobuf::TextFormat::Parse(&infile, &config)) {
    return { false, config };
  }
  return { true, config };
}

}
