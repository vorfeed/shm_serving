// Copyright 2017, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/shm_serving
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "shm_serving/ipc/segment_manager.h"

namespace shm_serving {

namespace ipc {

using bipc_string = boost::interprocess::basic_string<char, std::char_traits<char>,
    boost::interprocess::allocator<char, SegmentManager>>;

class string : public bipc_string {
public:
  string() :
    bipc_string(GlobalSegmentManager().get_allocator<char>()) {
  }

  string(const string& other) :
    bipc_string(other,
        GlobalSegmentManager().get_allocator<char>()) {
  }

  string(const char* cstring) :
    bipc_string(cstring,
        GlobalSegmentManager().get_allocator<char>()) {
  }

  string(const char* cstring, size_t size) :
    bipc_string(cstring, size,
        GlobalSegmentManager().get_allocator<char>()) {
  }

  string(const std::string& str) :
    bipc_string(str.data(), str.size(),
        GlobalSegmentManager().get_allocator<char>()) {
  }

  operator std::string() const {
    return std::string(this->c_str(), this->size());
  }
};

template <class T> using bipc_vector = boost::interprocess::vector<T,
    boost::interprocess::allocator<T, SegmentManager>>;

template <class T>
class vector : public bipc_vector<T> {
public:
  vector() :
    bipc_vector<T>(GlobalSegmentManager().get_allocator<T>()) {
  }
};

template <class Key, class Compare> using bipc_set =
    boost::interprocess::set<Key, Compare,
    boost::interprocess::allocator<Key, SegmentManager>>;

template <class Key, class Compare = std::less<Key>>
class set : public bipc_set<Key, Compare> {
public:
  set() :
    bipc_set<Key, Compare>(Compare(), GlobalSegmentManager().get_allocator<Key>()) {
  }
};

template <class Key, class Compare> using bipc_multiset =
    boost::interprocess::multiset<Key, Compare,
    boost::interprocess::allocator<Key, SegmentManager>>;

template <class Key, class Compare = std::less<Key>>
class multiset : public bipc_multiset<Key, Compare> {
public:
  multiset() :
    bipc_multiset<Key, Compare>(Compare(), GlobalSegmentManager().get_allocator<Key>()) {
  }
};

template <class Key> using bipc_unordered_set =
    boost::unordered_set<Key, std::hash<Key>, std::equal_to<Key>,
    boost::interprocess::allocator<Key, SegmentManager>>;

template <class Key>
class unordered_set : public bipc_unordered_set<Key> {
public:
  unordered_set() :
    bipc_unordered_set<Key>(GlobalSegmentManager().get_allocator<Key>()) {
  }
};

template <class Key, class Value> using bipc_unordered_map =
    boost::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>,
    boost::interprocess::allocator<std::pair<Key, Value>, SegmentManager>>;

template <class Key, class Value>
class unordered_map : public bipc_unordered_map<Key, Value> {
public:
  unordered_map() :
    bipc_unordered_map<Key, Value>(GlobalSegmentManager().get_allocator<std::pair<Key, Value>>()) {
  }
};

}

}

namespace std {

template<> struct hash<shm_serving::ipc::string> {
  size_t operator()(const shm_serving::ipc::string& str) const {
    return std::hash<std::string>()(str);
  }
};

}
