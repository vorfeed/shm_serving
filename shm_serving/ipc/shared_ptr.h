// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <boost/interprocess/smart_ptr/shared_ptr.hpp>

#include "shm_serving/ipc/shared_object.h"

namespace shm_serving {

namespace ipc {

template <class T> using shared_ptr = boost::interprocess::shared_ptr<T,
    SegmentManager::allocator<void>::type, typename SegmentManager::deleter<T>::type>;

template <class T, class... Args>
shared_ptr<T> make_shared(Args&&... args) {
  return *Construct<shared_ptr<T>>(
      Construct<T>(std::forward<Args>(args)...),
      GlobalSegmentManager().get_allocator<void>(),
      GlobalSegmentManager().get_deleter<T>());
}

}

}
