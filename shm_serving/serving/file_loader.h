// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_loader.h"
#include "shm_serving/shared/shared_storage.h"

namespace shm_serving {

DECLARE_string(data_dir);

template <class T>
class FileLoader : public IDataLoader {
 public:
  typedef T DataType;
  typedef SharedMemoryStore<T> DataStoreType;

  FileLoader(const std::string& file_name) : file_name_(file_name),
      store_(SharedStorage::GetInstance().CreateStore<SharedMemoryStore<T>>(file_name_)) {}

  ~FileLoader() override {
    SharedStorage::GetInstance().ReleaseStore<SharedMemoryStore<T>>(file_name_);
  }

  bool LoadBatchFile() override {
    std::ifstream in(FLAGS_data_dir + "/" + file_name_, std::ios::binary);
    if (!in) {
      return false;
    }
    ipc::shared_ptr<T> result(ipc::make_shared<T>());
    ParseFile(in, *result);
    store_.Update(result);
    LOG(INFO) << "[" << file_name_ << "]: load success, size: " << result->size();
    in.close();
    return true;
  }

 protected:
  virtual bool ParseFile(std::ifstream& in, T& result) = 0;

  const std::string file_name_;
  SharedMemoryStore<T>& store_;
};

}
