// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <sys/inotify.h>

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>

#include <boost/asio.hpp>
#include <glog/logging.h>
#include <gflags/gflags.h>

#include "shm_serving/serving/multiple_executor.h"

namespace shm_serving {

typedef std::function<void()> LoadCallback;

class EventMonitor {
 public:
  EventMonitor(const std::string& data_dir, MultipleExecutor& executor) :
    data_dir_(data_dir), executor_(executor),
    work_(io_service_), inotify_fd_(io_service_) {}

  bool Start() {
    int inotify_fd = inotify_init1(IN_NONBLOCK);
    if (inotify_fd < 0) {
      LOG(ERROR) << "inotify init failed!";
      return false;
    }
    inotify_fd_.assign(inotify_fd);
    thread_ = std::thread([this] {
      io_service_.run();
    });
    Monitor();
    return running_ = true;
  }

  void Stop() {
    running_ = false;
    io_service_.stop();
    thread_.join();
    if (inotify_fd_.is_open()) {
      for (const auto& watcher : file_fds_) {
        inotify_rm_watch(inotify_fd_.native_handle(), watcher.second);
      }
      file_fds_.clear();
      file_callbacks_.clear();
      inotify_fd_.close();
    }
  }

  bool RegisterFile(const std::string& file_name, LoadCallback callback,
      bool exclusive = false) {
    if (file_fds_.count(file_name)) {
      LOG(ERROR) << "file " << file_name << " already watched!";
      return false;
    }
    int watch_fd = inotify_add_watch(inotify_fd_.native_handle(),
        (data_dir_ + "/" + file_name).c_str(), IN_CLOSE_WRITE);
    if (watch_fd < 0) {
      LOG(ERROR) << "add watcher failed: " << file_name <<
          ", error: " << std::strerror(errno);
      return false;
    }
    file_fds_.emplace(file_name, watch_fd);
    boost::asio::io_service& ios = executor_.GetThread(file_name);
    file_callbacks_.emplace(watch_fd,
        std::make_tuple(file_name, callback, std::addressof(ios)));
    LOG(INFO) << "register watcher, file: " << file_name;
    ios.post(callback);
    return true;
  }

 private:
  void Monitor() {
    inotify_fd_.async_read_some(buffer_.prepare(4096),
        [this](boost::system::error_code ec, std::size_t bytes_transferred) {
      if (ec) {
        LOG(ERROR) << ec.message();
        return;
      }
      buffer_.commit(bytes_transferred);
      while(buffer_.size() >= sizeof(inotify_event)) {
        std::istream in(&buffer_);
        inotify_event iev;
        in.read(reinterpret_cast<char*>(&iev), sizeof(inotify_event));
        DLOG(INFO) << "event triggered: " << iev.mask;
        if (iev.mask != IN_CLOSE_WRITE) {
          buffer_.consume(sizeof(inotify_event) + iev.len);
          continue;
        }
        auto ite_watch_fd = file_callbacks_.find(iev.wd);
        if (ite_watch_fd == file_callbacks_.end()) {
          buffer_.consume(sizeof(inotify_event) + iev.len);
          continue;  // never reach here
        }
        boost::asio::io_service* io_service = std::get<2>(ite_watch_fd->second);
        LoadCallback& callback = std::get<1>(ite_watch_fd->second);
        io_service->post(callback);
        buffer_.consume(sizeof(inotify_event) + iev.len);
      }
      Monitor();
    });
  }

  const std::string data_dir_;
  MultipleExecutor& executor_;
  boost::asio::io_service io_service_;
  boost::asio::io_service::work work_;
  boost::asio::posix::stream_descriptor inotify_fd_;
  std::atomic_bool running_ { false };
  std::unordered_map<std::string, int> file_fds_;
  std::unordered_map<int, std::tuple<std::string, LoadCallback,
      boost::asio::io_service*>> file_callbacks_;
  std::thread thread_;
  boost::asio::streambuf buffer_;
};

}
