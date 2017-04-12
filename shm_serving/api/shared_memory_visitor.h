// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include "shm_serving/ipc/ipc.h"

namespace shm_serving {

class SharedMemoryVisitor {
 public:
  SharedMemoryVisitor(const std::string& name) :
      segment_(bipc::open_only, name.c_str()) {
    ipc::GlobalSegmentManager(segment_.get_segment_manager());
  }

 private:
  bipc::managed_shared_memory segment_;
};

}
