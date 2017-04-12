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
class SharedSwitchoverStore {
 public:
  template <class... Args>
  SharedSwitchoverStore(Args&&... args) :
      main_(ipc::make_shared<T>(args...)),
      standby_(ipc::make_shared<T>(std::forward<Args>(args)...)) {}

  ipc::shared_ptr<T> Main() {
    ReadLock lock(mutex_);
    return main_;
  }

  ipc::shared_ptr<T> Standby() {
    ReadLock lock(mutex_);
    return standby_;
  }

  ipc::shared_ptr<T> SafeStandby() {
    while (!IsStandbySafe()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return Standby();
  }

  void Switch() {
    WriteLock lock(mutex_);
    main_.swap(standby_);
  }

  void SwitchCopy() {
    Switch();
    ipc::shared_ptr<T> standby = SafeStandby();
    ipc::shared_ptr<T> main = Main();
    *standby = *main;
  }

 private:
  typedef boost::shared_lock<boost::shared_mutex> ReadLock;
  typedef boost::unique_lock<boost::shared_mutex> WriteLock;

  bool IsStandbySafe() {
    ReadLock lock(mutex_);
    return standby_.use_count() == 2;
  }

  ipc::shared_ptr<T> main_, standby_;
  boost::shared_mutex mutex_;
};

}
