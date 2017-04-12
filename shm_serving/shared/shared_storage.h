// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/shared/shared_memory_store.h"
#include "shm_serving/shared/shared_switchover_store.h"
#include "shm_serving/ipc/ipc.h"

namespace shm_serving {

class SharedStorage {
 public:
  static SharedStorage& GetInstance() {
    return *ipc::FindByName<SharedStorage>("shared_storage");
  }

  template <class Store>
  Store& CreateStore(const std::string& file_name) {
    assert(!shared_stores_.count(file_name));
    bipc::offset_ptr<Store> store(ipc::Construct<Store>());
    shared_stores_.emplace(file_name, std::make_pair(store, typeid(Store).hash_code()));
    return *store;
  }

  template <class Store>
  void ReleaseStore(const std::string& file_name) {
    assert(shared_stores_.count(file_name));
    auto& store_info = shared_stores_[file_name];
    assert(store_info.second == typeid(Store).hash_code());
    Store* store = reinterpret_cast<Store*>(store_info.first.get());
    ipc::Release(store);
  }

  template <class Data>
  const ipc::shared_ptr<Data> GetData(const std::string& file_name) {
    auto ite_store = shared_stores_.find(file_name);
    if (ite_store == shared_stores_.end()) {
      LOG(ERROR) << "no data store found: " << file_name;
      return {};
    }
    size_t hash_code = ite_store->second.second;
    if (typeid(SharedMemoryStore<Data>).hash_code() == hash_code) {
      SharedMemoryStore<Data>* store =
          reinterpret_cast<SharedMemoryStore<Data>*>(ite_store->second.first.get());
      return store->Data();
    } else if (typeid(SharedSwitchoverStore<Data>).hash_code() == hash_code) {
      SharedSwitchoverStore<Data>* store =
          reinterpret_cast<SharedSwitchoverStore<Data>*>(ite_store->second.first.get());
      return store->Main();
    }
    LOG(ERROR) << "data class mismatch: " << file_name;
    return {};
  }

 private:
  friend class DataContainer;

  ipc::unordered_map<ipc::string, std::pair<bipc::offset_ptr<void>, size_t>> shared_stores_;

 public:
  SharedStorage() = default;
};

}
