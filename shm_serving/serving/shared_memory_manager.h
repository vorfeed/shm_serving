// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include "shm_serving/ipc/ipc.h"

namespace shm_serving {

class SharedMemoryManager {
 public:
  // TODO: singleton
  SharedMemoryManager(const std::string name, size_t capacity, bool auto_delete = false) :
      name_(name),
      shared_memory_remover_(auto_delete ? name : ""),
      segment_(bipc::open_or_create, name_.c_str(), capacity) {
    ipc::GlobalSegmentManager(segment_.get_segment_manager());
  }

 private:
  class SharedMemoryRemover {
   public:
    SharedMemoryRemover(const std::string& name = "") : name_(name) {
      if (!name_.empty()) {
        bipc::shared_memory_object::remove(name_.c_str());
      }
    }
    ~SharedMemoryRemover() {
      if (!name_.empty()) {
        bipc::shared_memory_object::remove(name_.c_str());
      }
    }
   private:
    std::string name_;
  };

  std::string name_;
  SharedMemoryRemover shared_memory_remover_;
  bipc::managed_shared_memory segment_;
};

}
