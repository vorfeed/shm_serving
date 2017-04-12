// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include "shm_serving/shared/data_container.h"

namespace shm_serving {

// not thread safe
class DataContainerHolder {
 public:
  DataContainerHolder() {
    static bool created = false;
    if (!created) {
      DataContainer::Init();
      created = true;
    }
  }

  ~DataContainerHolder() {
    DataContainer::Release();
  }
};

}
