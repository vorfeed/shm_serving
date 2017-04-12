// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <string>
#include <unordered_map>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_loader.h"
#include "shm_serving/serving/config/loader_factory.h"
#include "shm_serving/serving/proto/config.pb.h"
#include "shm_serving/serving/task_dispatcher.h"
#include "shm_serving/serving/util/protobuf_config_parser.h"

namespace shm_serving {

DECLARE_string(config_dir);
DECLARE_string(config_file);

class LoaderManager {
 public:
  LoaderManager(TaskDispatcher& task_dispatcher) :
      task_dispatcher_(task_dispatcher) {}

  bool RegisterAllLoaders() {
    auto ret = ParseConfig<DataServiceConfig>(FLAGS_config_dir + "/" + FLAGS_config_file);
    if (!ret.first) {
      LOG(ERROR) << "parse data service config failed, file: " << FLAGS_config_file;
      return false;
    }
    for (const NamedLoader& named_loader : ret.second.named_loaders()) {
      if (!RegisterLoader(named_loader.file_name(), named_loader.exclusive())) {
        return false;
      }
    }
    return true;
  }

  bool RegisterLoader(const std::string& file_name, bool exclusive = false) {
    std::shared_ptr<IDataLoader> loader = LoaderFactory::CreateLoader(file_name);
    if (!loader) {
      return false;
    }
    loaders_.emplace(file_name, loader);
    task_dispatcher_.RegisterTask(file_name, loader->GetBatchLoaderCallback(), loader->GetIncrLoaderCallback(), exclusive);
    LOG(INFO) << "register loader: " << file_name;
    return true;
  }

 private:
  TaskDispatcher& task_dispatcher_;
  std::unordered_map<std::string, std::shared_ptr<IDataLoader>> loaders_;
};

}
