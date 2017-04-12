// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <thread>

#include <boost/asio.hpp>

namespace shm_serving {

struct AsioThread {
  AsioThread() : work(io_service), thread([this]() {
    io_service.run();
  }) {}
  ~AsioThread() {
    io_service.stop();
    thread.join();
  }
  boost::asio::io_service io_service;
  boost::asio::io_service::work work;
  std::thread thread;
};

}
