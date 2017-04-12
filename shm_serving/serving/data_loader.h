// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <functional>

#include <gflags/gflags.h>

namespace shm_serving {

DECLARE_string(data_dir);

typedef std::function<bool()> BatchLoadCallback, IncrLoaderCallback;

namespace detail {

class ISplitableLoader {
 public:
  virtual ~ISplitableLoader() = default;

  virtual BatchLoadCallback GetBatchLoaderCallback() = 0;

  virtual IncrLoaderCallback GetIncrLoaderCallback() { return nullptr; }
};

}

class IDataLoader : public detail::ISplitableLoader {
 public:
  virtual ~IDataLoader() = default;

  virtual bool LoadBatchFile() = 0;

  BatchLoadCallback GetBatchLoaderCallback() override {
    return std::bind(&IDataLoader::LoadBatchFile, this);
  }
};

}
