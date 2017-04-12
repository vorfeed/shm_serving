// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>

namespace shm_serving {

namespace ipc {

using SegmentManager = boost::interprocess::managed_shared_memory::segment_manager;

SegmentManager& GlobalSegmentManager(SegmentManager* segment_manager = nullptr) {
  static SegmentManager* global_segment_manager(segment_manager);
  return *global_segment_manager;
}

}

}
