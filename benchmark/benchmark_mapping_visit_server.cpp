// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <chrono>
#include <fstream>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/shared_memory_manager.h"

using namespace std;
using namespace shm_serving;

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  SharedMemoryManager shared_memory_manager("MappingVisit", 1L << 36, true);
  using StringMap = ipc::unordered_map<ipc::string, ipc::string>;
  StringMap* string_map = ipc::GlobalSegmentManager().construct<StringMap>("StringMap")();
  auto start = std::chrono::steady_clock::now();
  std::ifstream in("mapping_data", std::ios::binary);
  std::string line;
  std::vector<std::string> segments;
  int count = 0;
  while (in) {
    getline(in, line);
    if (++count % 100000 == 0) {
      std::cout << "load " << count << " lines." << std::endl;
    }
    if (line.empty()) {
      continue;
    }
    boost::split(segments, line, boost::is_any_of("\t"));
    if (segments.size() < 2) {
      std::cout << "segments.size() < 2, line: " << count << std::endl;
    }
    string_map->emplace(segments[0], segments[1]);
  }
  in.close();
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> used = end - start;
  std::cout << "load time used: " << used.count() << std::endl;
  start = std::chrono::steady_clock::now();
  std::unordered_map<std::string, std::string> local_map;
  for (const auto& kv : *string_map) {
    std::string key(kv.first);
    std::string value(kv.second);
    local_map.emplace(key, value);
  }
  end = std::chrono::steady_clock::now();
  used = end - start;
  std::cout << "local visit time used: " << used.count() << std::endl;
  sleep(-1);
}
