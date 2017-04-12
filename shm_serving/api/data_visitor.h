// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include "shm_serving/shared/data_container.h"

namespace shm_serving {

class DataVisitor {
 public:
  DataVisitor() : data_container_(DataContainer::GetInstance()) {}

  template <class Data>
  const ipc::shared_ptr<Data> GetData(const std::string& file_name) {
    return data_container_.GetData<Data>(file_name);
  }

 private:
  DataContainer& data_container_;
};

}
