/*
 * Copyright (C) 2021-2026 The VILA Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/****************************************
 * Description: modified from eigen
 ****************************************/

// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2018 Rasmus Munk Larsen <rmlarsen@google.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Barrier is an object that allows one or more threads to wait until
// Notify has been called a specified number of times.

#ifndef CLIM_BARRIER_H_
#define CLIM_BARRIER_H_
#include <stdint.h>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

/**
 * @brief A waitable object that can be blocked until been notified for
 * `count` times.
 */
class Barrier {
 public:
  Barrier(uint32_t count) : state_(count << 1), notified_(false) {
    assert(((count << 1) >> 1) == count);
  }

  ~Barrier() { assert((state_ >> 1) == 0); }

  /// Signal the object once
  void Notify() {
    uint32_t v = state_.fetch_sub(2, std::memory_order_acq_rel) - 2;
    if (v != 1) {
      // Clear the lowest bit (waiter flag) and check that the original state
      // value was not zero. If it was zero, it means that notify was called
      // more times than the original count.
      assert(((v + 2) & ~1) != 0);
      return;  // either count has not dropped to 0, or waiter is not waiting
    }
    std::unique_lock<std::mutex> l(mu_);
    assert(!notified_);
    notified_ = true;
    cv_.notify_all();
  }

  /// Wait until the object has been waked up.
  void Wait() {
    uint32_t v = state_.fetch_or(1, std::memory_order_acq_rel);
    if ((v >> 1) == 0) return;
    std::unique_lock<std::mutex> l(mu_);
    while (!notified_) {
      cv_.wait(l);
    }
  }

  /// Try wait for a period of time, return false if timeout.
  template <class Rep, class Period>
  bool WaitFor(const std::chrono::duration<Rep, Period>& timeout) {
    uint32_t v = state_.fetch_or(1, std::memory_order_acq_rel);
    if ((v >> 1) == 0) return true;
    std::unique_lock<std::mutex> l(mu_);
    while (!notified_) {
      if (cv_.wait_for(l, timeout) == std::cv_status::timeout) {
        return false;
      }
    }
    return true;
  }

  /// Whether this object is been notified
  bool Notified() const noexcept { return notified_; }

 private:
  std::mutex mu_;
  std::condition_variable cv_;
  std::atomic<uint32_t> state_;  // low bit is waiter flag
  bool notified_;
};

#endif  // CLIM_BARRIER_H_
