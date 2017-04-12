// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_loader.h"
#include "shm_serving/serving/event_monitor.h"
#include "shm_serving/serving/multiple_executor.h"
#include "shm_serving/serving/util/chrono_timer.h"

namespace shm_serving {

struct IncrConfig {
  IncrConfig() = default;
  IncrConfig(int incr_round_interval_s, int incr_retry_interval_s) :
    incr_round_interval_s(incr_round_interval_s),
    incr_retry_interval_s(incr_retry_interval_s) {}
  int incr_round_interval_s { 60 };
  int incr_retry_interval_s { 10 };
};

class TaskDispatcher {
 public:
  TaskDispatcher(EventMonitor& event_monitor, MultipleExecutor& executor) :
      event_monitor_(event_monitor), executor_(executor) {}

  // TODO: move
  bool RegisterTask(const std::string& file_name, BatchLoadCallback batch_loader,
      IncrLoaderCallback incr_loader = nullptr, bool exclusive = false,
      const IncrConfig& incr_config = IncrConfig()) {
    boost::asio::io_service& io_service = exclusive ?
        executor_.RegisterPooledThread(file_name) : executor_.RegisterTaggedThread(file_name);
    SteadyTimer& timer = executor_.GetTimer(file_name);
    if (!event_monitor_.RegisterFile(file_name,
        [this, file_name, &io_service, &timer, batch_loader, incr_loader]() {
      io_service.post([this, file_name, &io_service, &timer, batch_loader, incr_loader]() {
        if (!batch_loader()) {
          // load failed
          return;
        }
        if (!incr_loader) {
          return;
        }
        // TODO: use auto decltype of recursive lambda function types in C++14
        TimedIncr(file_name, timer, incr_loader);
      });
    })) {
      return false;
    }
    incr_configs_.emplace(file_name, incr_config);
    return true;
  }

 private:
  void TimedIncr(const std::string& file_name,
      SteadyTimer& timer, IncrLoaderCallback incr_loader) {
    bool incr_retry = !incr_loader();
    timer.expires_from_now(std::chrono::seconds(incr_retry ?
        incr_configs_[file_name].incr_retry_interval_s :
        incr_configs_[file_name].incr_round_interval_s));
    timer.async_wait(std::bind([this, file_name, &timer, incr_loader](const boost::system::error_code& ec, uint64_t version) {
      if (ec) {
        // TODO:
      }
      if (version < round_versions_[file_name]) {
        LOG(WARNING) << "overdue task detected, task version: " << version <<
            ", current version: " << round_versions_[file_name] << ", discard!";
        return;
      }
      TimedIncr(file_name, timer, incr_loader);
    }, std::placeholders::_1, incr_retry ? round_versions_[file_name] : ++round_versions_[file_name]));
  }

  EventMonitor& event_monitor_;
  MultipleExecutor& executor_;
  std::chrono::seconds incr_interval_s_;
  std::unordered_map<std::string, uint64_t> round_versions_;
  std::unordered_map<std::string, IncrConfig> incr_configs_;
};

}
