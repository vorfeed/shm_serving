// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <string>
#include <unordered_map>

#include <gflags/gflags.h>

#include "shm_serving/ipc/ipc.h"
#include "shm_serving/shared/shared_storage.h"

namespace shm_serving {

class DataContainer {
 public:
  // XXX: called after data_container is inited.
  static DataContainer& GetInstance() {
    return *ipc::FindByName<DataContainer>(GetContainerName());
  }

  template <class Data>
  const ipc::shared_ptr<Data> GetData(const std::string& file_name) {
    return shared_storage_->GetData<Data>(file_name);
  }

 private:
  friend class DataContainerHolder;

  DataContainer(const DataContainer&) = delete;
  DataContainer& operator=(const DataContainer&) = delete;

  // XXX: called by DataContainerHolder only
  static DataContainer& Init() {
    DataContainer& data_container = *ipc::NamedConstruct<DataContainer>(GetContainerName());
    return data_container;
  }

  static void Release() {
    ipc::Release(ipc::FindByName<DataContainer>(GetContainerName()));
  }

  bipc::offset_ptr<SharedStorage> shared_storage_;

 public:
  static const std::string GetContainerName() {
    static const std::string KContainerName = "data_container";
    return KContainerName;
  }

  // XXX: boost::interprocess::ipcdetail::CtorNArg<T, is_iterator, Args>::construct_n needs public construtor
  // never call this
  DataContainer() : shared_storage_(ipc::NamedConstruct<SharedStorage>("shared_storage")) {}
};

}
