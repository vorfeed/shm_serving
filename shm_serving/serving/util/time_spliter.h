// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <cmath>
#include <ctime>

namespace shm_serving {

inline std::string FormatTime(time_t t) {
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
  return buff;
}

inline int GetIncrBeginTime(const std::string& file_path) {
  boost::filesystem::path path(file_path);
  std::time_t time = -1;
  try {
    time = boost::filesystem::last_write_time(path);
  } catch (boost::filesystem::filesystem_error &e) {
    return -1;
  }
  std::tm tm;
  localtime_r(&time, &tm);
  tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
  return std::mktime(&tm);
}

inline std::vector<std::pair<int, int>> GetTimeSegments(
    const std::string& file_path, int split_interval_s) {
  std::vector<std::pair<int, int>> time_segments;
  int begin_time = GetIncrBeginTime(file_path);
  if (begin_time < 0) {
    return time_segments;
  }
  int end_time = std::time(nullptr);
  if (end_time <= begin_time) {
    return time_segments;
  }
  time_segments.resize(std::ceil((end_time - begin_time) * 1.0 / split_interval_s));
  int step = 0;
  for (auto& segment : time_segments) {
    segment.first = begin_time + step;
    step += split_interval_s;
    segment.second = begin_time + step;
  }
  time_segments.back().second = std::min(end_time, time_segments.back().second);
  return time_segments;
}

}
