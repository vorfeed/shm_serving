// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "shm_serving/serving/data_loader.h"
#include "shm_serving/serving/util/time_spliter.h"
#include "shm_serving/shared/shared_storage.h"

namespace shm_serving {

DECLARE_string(data_dir);
DECLARE_int32(incr_split_interval_s);
DECLARE_int32(incr_retry_interval_s);
DECLARE_bool(disable_incr);

template <class T>
class FileIncrementalLoader : public IDataLoader {
 public:
  typedef T DataType;
  typedef SharedSwitchoverStore<T> DataStoreType;

  FileIncrementalLoader(const std::string& file_name) : file_name_(file_name),
      store_(SharedStorage::GetInstance().CreateStore<SharedSwitchoverStore<T>>(file_name_)) {}

  ~FileIncrementalLoader() override {
    SharedStorage::GetInstance().ReleaseStore<SharedSwitchoverStore<T>>(file_name_);
  }

  IncrLoaderCallback GetIncrLoaderCallback() override {
    return std::bind(&FileIncrementalLoader::LoadIncrInfo, this);
  }

  bool LoadBatchFile() override {
    std::string file_path = FLAGS_data_dir + "/" + file_name_;
    std::ifstream in(file_path, std::ios::binary);
    if (!in) {
      return false;
    }
    auto standby = store_.SafeStandby();
    standby->clear();
    ParseFile(in, *standby);
    LOG(INFO) << "[" << file_name_ << "]: batch success, size: " << standby->size();
    std::vector<std::pair<int, int>> time_segments =
        GetTimeSegments(file_path, FLAGS_incr_split_interval_s);
    if (!FLAGS_disable_incr && !time_segments.empty()) {
      for (const auto& time_segment : time_segments) {
        while (!LoadIncr(time_segment)) {
          std::this_thread::sleep_for(std::chrono::seconds(FLAGS_incr_retry_interval_s));
          LOG(WARNING) << "[" << file_name_ << "]: (" << FormatTime(time_segment.first) << "," <<
              FormatTime(time_segment.second) << "), incr load failed!";
        }
      }
      MergeIncr(*standby);
      last_fetched_time_s_ = time_segments.back().second;
    }
    store_.SwitchCopy();
    LOG(INFO) << "[" << file_name_ << "]: incr finished, size: " << standby->size();
    in.close();
    return true;
  }

  virtual bool LoadIncrInfo() {
    if (FLAGS_disable_incr) {
      return true;
    }
    auto standby = store_.SafeStandby();
    std::pair<int32_t,int32_t> time_segment = std::make_pair(last_fetched_time_s_, std::time(nullptr));
    if (!LoadIncr(time_segment)) {
      // task dispatcher will retry
      // TODO: cut long time into segments, low priority
      LOG(WARNING) << "[" << file_name_ << "]: (" << FormatTime(time_segment.first) << "," <<
          FormatTime(time_segment.second) << "), incr load failed!";
      return false;
    }
    last_fetched_time_s_ = time_segment.second;
    MergeIncr(*standby);
    store_.Switch();
    standby = store_.SafeStandby();
    MergeIncr(*standby);
    LOG(INFO) << "[" << file_name_ << "]: current size: " << standby->size();
    return true;
  }

 protected:
  virtual bool ParseFile(std::ifstream& in, T& result) = 0;
  virtual bool LoadIncr(const std::pair<int, int>& time_segment) = 0;
  virtual bool MergeIncr(T& result) = 0;

  const std::string file_name_;
  SharedSwitchoverStore<T>& store_;
  int last_fetched_time_s_ { -1 };
};

}
