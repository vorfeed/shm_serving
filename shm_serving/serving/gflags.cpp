// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <gflags/gflags.h>

namespace shm_serving {

DEFINE_string(shm_serving_name, "shm_serving", "");
DEFINE_int64(shared_memory_capacity_gb, 1L << 6, "");
DEFINE_bool(disable_incr, false, "");
DEFINE_int32(incr_split_interval_s, 1800, "");
DEFINE_int32(incr_retry_interval_s, 10, "");
DEFINE_string(data_dir, "/home/xiaojie/shm_serving/data", "");
DEFINE_string(config_dir, "/home/xiaojie/shm_serving/conf", "");
DEFINE_string(config_file, "shm_serving.conf", "");

}
