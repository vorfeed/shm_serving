// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include "shm_serving/api/data_visitor.h"
#include "shm_serving/api/shared_memory_visitor.h"

using namespace std;
using namespace shm_serving;

namespace shm_serving {

DECLARE_string(shm_serving_name);

}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  SharedMemoryVisitor shared_memory_visitor(FLAGS_shm_serving_name);
  DataVisitor data_visitor;
  auto line_data = data_visitor.GetData<ipc::unordered_set<ipc::string>>("line_data");
  if (!line_data) {
    cerr << "visit line_data failed!" << endl;
  } else {
    cout << "visit line_data succeed." << endl;
  }
  if (line_data->size() != 6ul ||
      !line_data->count("333333") ||
      !line_data->count("666666")) {
    cerr << "check line_data failed!" << endl;
  } else {
    cout << "check line_data succeed." << endl;
  }
}
