// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <memory>

#if __cplusplus < 201402L

namespace std {

template <class T, class... Args>
inline unique_ptr<T> make_unique(Args&&... args) {
  return unique_ptr<T>(new T(forward<Args>(args)...));
}

}

#endif
