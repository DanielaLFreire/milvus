// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <mutex>

namespace knowhere {
namespace algo {

using node_t = int64_t;

// TODO: search use simple neighbor
struct Neighbor {
    node_t id;  // offset of node in origin data
    float distance;
    bool has_explored;

    Neighbor() = default;

    explicit Neighbor(node_t id, float distance, bool f) : id{id}, distance{distance}, has_explored(f) {
    }

    explicit Neighbor(node_t id, float distance) : id{id}, distance{distance}, has_explored(false) {
    }

    inline bool
    operator<(const Neighbor& other) const {
        return distance < other.distance;
    }
};

// struct SimpleNeighbor {
//    node_t id; // offset of node in origin data
//    float distance;
//
//    SimpleNeighbor() = default;
//    explicit SimpleNeighbor(node_t id, float distance) : id{id}, distance{distance}{}
//
//    inline bool operator<(const Neighbor &other) const {
//        return distance < other.distance;
//    }
//};

typedef std::lock_guard<std::mutex> LockGuard;

}  // namespace algo
}  // namespace knowhere
