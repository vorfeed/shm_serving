// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include "shm_serving/ipc/segment_manager.h"

namespace shm_serving {

namespace ipc {

template <class T, class... Args>
T* Construct(Args&&... args) {
  return GlobalSegmentManager().construct<T>(
      boost::interprocess::anonymous_instance)(std::forward<Args>(args)...);
}

template <class T, class... Args>
T* NamedConstruct(const std::string& name, Args&&... args) {
  return GlobalSegmentManager().construct<T>(
      name.c_str())(std::forward<Args>(args)...);
}

template <class T>
void Release(const T* object) {
  GlobalSegmentManager().destroy_ptr(object);
}

template <class T>
T* FindByName(const std::string& name) {
  return GlobalSegmentManager().find<T>(name.c_str()).first;
}

}

}
