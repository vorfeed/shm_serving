// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <signal.h>
#include <future>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "shm_serving/serving/data_manager.h"

DEFINE_bool(daemon, false, "");
DECLARE_int32(minloglevel);
DECLARE_string(log_dir);

std::promise<void> promise;

int main(int argc, char* argv[]) {
  FLAGS_minloglevel = 0;
  FLAGS_log_dir = "/home/xiaojie/shm_serving/glog";
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, [](int signal) {
    shm_serving::DataManager::GetInstance().Stop();
    promise.set_value();
  });
  if (FLAGS_daemon) {
    daemon(0, 0);
  }
  shm_serving::DataManager::GetInstance().Start();
  promise.get_future().get();
}
