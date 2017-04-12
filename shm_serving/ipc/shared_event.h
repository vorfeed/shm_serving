// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace shm_serving {

namespace ipc {

class SharedEvent {
 public:
  void Signal() {
    {
      Lock lock(mutex_);
      signaled_ = true;
    }
    condition_.notify_one();
  }

  void Wait() {
    Lock lock(mutex_);
    condition_.wait(lock, [this] { return signaled_; });
  }

 private:
  typedef boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> Lock;

  boost::interprocess::interprocess_mutex mutex_;
  boost::interprocess::interprocess_condition condition_;
  bool signaled_ { false };
};

}

}
