// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <chrono>
#include <thread>

#include <boost/thread/shared_mutex.hpp>

#include "shm_serving/ipc/ipc.h"

namespace shm_serving {

template <class T>
class SharedMemoryStore {
 public:
  template <class... Args>
  SharedMemoryStore(Args&&... args) :
      data_(ipc::make_shared<T>(std::forward<Args>(args)...)) {}

  ipc::shared_ptr<T> Data() {
    ReadLock lock(mutex_);
    return data_;
  }

  bool Update(const ipc::shared_ptr<T>& data) {
    assert(data);
    ipc::shared_ptr<T> hold(data_);
    WriteLock lock(mutex_);
    data_ = data;
    return true;
  }

 private:
  typedef boost::shared_lock<boost::shared_mutex> ReadLock;
  typedef boost::unique_lock<boost::shared_mutex> WriteLock;

  ipc::shared_ptr<T> data_;
  boost::shared_mutex mutex_;
};

}
