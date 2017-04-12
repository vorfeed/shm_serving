// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_loader.h"
#include "shm_serving/serving/data_loader/line_loader.h"

namespace shm_serving {

class LoaderFactory {
 public:
  static std::shared_ptr<IDataLoader> CreateLoader(const std::string& file_name) {
    static std::unordered_map<std::string, std::function<std::shared_ptr<IDataLoader>(const std::string&)>> loader_creators = {
        { "line_data", std::make_shared<LineLoader, const std::string&> },
    };
    auto ite = loader_creators.find(file_name);
    if (ite == loader_creators.end()) {
      LOG(ERROR) << "no available loader found for file: " << file_name;
      return {};
    }
    return ite->second(file_name);
  }
};

}
