// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include <boost/asio.hpp>

#include "shm_serving/serving/util/asio_thread.h"
#include "shm_serving/serving/util/chrono_timer.h"
#include "shm_serving/serving/util/version_adaptor.h"

namespace shm_serving {

class MultipleExecutor {
 public:
  MultipleExecutor(size_t pooled_thread_num = 4) :
    pooled_threads_(pooled_thread_num) {}

  MultipleExecutor(const MultipleExecutor&) = delete;
  MultipleExecutor& operator=(const MultipleExecutor&) = delete;

  boost::asio::io_service& RegisterPooledThread(const std::string& tag) {
    auto& pooled_thread =
        pooled_threads_[TagHash(tag) % pooled_threads_.size()];
    timers_.emplace(tag, std::make_unique<SteadyTimer>(
        pooled_threads_[TagHash(tag) % pooled_threads_.size()].io_service));
    return pooled_thread.io_service;
  }

  boost::asio::io_service& RegisterTaggedThread(const std::string& tag) {
    auto& tagged_thread = tagged_threads_[tag];
    timers_.emplace(tag, std::make_unique<SteadyTimer>(
        tagged_threads_[tag].io_service));
    return tagged_thread.io_service;
  }

  boost::asio::io_service& GetThread(const std::string& tag) {
    assert(timers_.count(tag));
    return tagged_threads_.count(tag) ? tagged_threads_[tag].io_service :
        pooled_threads_[TagHash(tag) % pooled_threads_.size()].io_service;
  }

  SteadyTimer& GetTimer(const std::string& tag) {
    assert(timers_.count(tag));
    return *timers_.find(tag)->second;
  }

 private:
  size_t TagHash(std::string tag) const {
    return std::hash<std::string>()(tag);
  }

  std::vector<AsioThread> pooled_threads_;
  std::unordered_map<std::string, AsioThread> tagged_threads_;
  std::unordered_map<std::string, std::unique_ptr<SteadyTimer>> timers_;
};

}
