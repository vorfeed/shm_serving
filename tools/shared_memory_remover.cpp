// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

using namespace std;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  if (argc < 2) {
    boost::interprocess::shared_memory_object::remove("shm_serving");
    cout << "shared memory shm_serving removed." << endl;
    return 0;
  }
  for (int i = 1; i < argc; ++i) {
    boost::interprocess::shared_memory_object::remove(argv[i]);
    cout << "shared memory " << argv[i] << " removed." << endl;
  }
}
