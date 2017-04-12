// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <fstream>

#include "shm_serving/ipc/ipc.h"
#include "shm_serving/serving/file_loader.h"

namespace shm_serving {

class LineLoader : public FileLoader<ipc::unordered_set<ipc::string>> {
 public:
  using FileLoader<ipc::unordered_set<ipc::string>>::FileLoader;

  virtual ~LineLoader() = default;

 protected:
  bool ParseFile(std::ifstream& in, ipc::unordered_set<ipc::string>& result) override {
    while (in) {
      std::string line;
      getline(in, line);
      if (!line.empty()) {
        result.emplace(line);
      }
    }
    return true;
  }
};

}
