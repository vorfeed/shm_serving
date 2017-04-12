// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <memory>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_container_holder.h"
#include "shm_serving/serving/event_monitor.h"
#include "shm_serving/serving/loader_manager.h"
#include "shm_serving/serving/multiple_executor.h"
#include "shm_serving/serving/shared_memory_manager.h"
#include "shm_serving/serving/task_dispatcher.h"

namespace shm_serving {

DECLARE_string(data_dir);
DECLARE_string(shm_serving_name);
DECLARE_int64(shared_memory_capacity_gb);

class DataManager {
 public:
  static DataManager& GetInstance() {
    static DataManager data_manager;
    return data_manager;
  }

  bool Start() {
    shared_memory_manager_.reset(new SharedMemoryManager(FLAGS_shm_serving_name,
        FLAGS_shared_memory_capacity_gb * (1L << 30)));
    data_container_holder_.reset(new DataContainerHolder());
    executor_.reset(new MultipleExecutor());
    data_monitor_.reset(new EventMonitor(FLAGS_data_dir, *executor_));
    data_monitor_->Start();
    task_dispatcher_.reset(new TaskDispatcher(*data_monitor_, *executor_));
    loader_manager_.reset(new LoaderManager(*task_dispatcher_));
    loader_manager_->RegisterAllLoaders();
    return true;
  }

  void Stop() {
    data_monitor_->Stop();
    loader_manager_.reset();
    task_dispatcher_.reset();
    executor_.reset();
    data_monitor_.reset();
    data_container_holder_.reset();
    shared_memory_manager_.reset();
  }

 private:
  DataManager() = default;

  std::shared_ptr<SharedMemoryManager> shared_memory_manager_;
  std::shared_ptr<DataContainerHolder> data_container_holder_;
  std::shared_ptr<EventMonitor> data_monitor_;
  std::shared_ptr<EventMonitor> hbase_conf_monitor_;
  std::shared_ptr<MultipleExecutor> executor_;
  std::shared_ptr<TaskDispatcher> task_dispatcher_;
  std::shared_ptr<LoaderManager> loader_manager_;
};

}
