// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <chrono>
#include <unordered_map>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/api/shared_memory_visitor.h"

using namespace std;
using namespace shm_serving;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  SharedMemoryVisitor shared_memory_visitor("MappingVisit");
  using StringMap = ipc::unordered_map<ipc::string, ipc::string>;
  StringMap* string_map = ipc::GlobalSegmentManager().find<StringMap>("StringMap").first;
  auto start = std::chrono::steady_clock::now();
  std::unordered_map<std::string, std::string> local_map;
  for (const auto& kv : *string_map) {
    std::string key(kv.first);
    std::string value(kv.second);
    local_map.emplace(key, value);
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> used = end - start;
  std::cout << "shared visit time used: " << used.count() << std::endl;
}
