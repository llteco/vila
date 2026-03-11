/*
 * Copyright (C) 2023-2026 The VILA Authors.
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
 *
 */
#ifndef VILA_STATUS_STATUSOR_INTERNALS_H_
#define VILA_STATUS_STATUSOR_INTERNALS_H_
#include <utility>

#include "vila/status/status.h"

namespace vila {
namespace internal_statusor {
// Construct an instance of T in `p` through placement new, passing Args... to
// the constructor.
// This abstraction is here mostly for the gcc performance fix.
template <typename T, typename... Args>
void PlacementNew(void* p, Args&&... args) {
#if defined(__GNUC__) && !defined(__clang__)
  // Teach gcc that 'p' cannot be null, fixing code size issues.
  if (p == nullptr) __builtin_unreachable();
#endif
  new (p) T(std::forward<Args>(args)...);
}

// Helper base class to hold the data and all operations.
// We move all this to a base class to allow mixing with the appropriate
// TraitsBase specialization.
template <typename T>
class StatusOrData {
  template <typename U>
  friend class StatusOrData;

 public:
  StatusOrData() = delete;

  StatusOrData(const StatusOrData& other) {
    if (other.Ok()) {
      makeValue(other.data_);
      makeStatus();
    } else {
      makeStatus(other.status_);
    }
  }

  StatusOrData(StatusOrData&& other) noexcept {
    if (other.Ok()) {
      makeValue(std::move(other.data_));
      makeStatus();
    } else {
      makeStatus(std::move(other.status_));
    }
  }

  template <typename U>
  explicit StatusOrData(const StatusOrData<U>& other) {
    if (other.Ok()) {
      makeValue(other.data_);
      makeStatus();
    } else {
      makeStatus(other.status_);
    }
  }

  template <typename U>
  explicit StatusOrData(StatusOrData<U>&& other) {
    if (other.Ok()) {
      makeValue(std::move(other.data_));
      makeStatus();
    } else {
      makeStatus(std::move(other.status_));
    }
  }

  explicit StatusOrData(const T& value) : data_(value) { makeStatus(); }

  explicit StatusOrData(T&& value) : data_(std::move(value)) { makeStatus(); }

  template <
      class U,
      std::enable_if_t<std::is_constructible<Status, U&&>::value, int> = 0>
  explicit StatusOrData(U&& status) : status_(std::forward<U>(status)) {
    ensureNotOk();
  }

  StatusOrData& operator=(const StatusOrData& other) {
    if (this == &other) return *this;
    if (other.Ok()) {
      Assign(other.data_);
    } else {
      AssignStatus(other.status_);
    }
    return *this;
  }

  StatusOrData& operator=(StatusOrData&& other) noexcept {
    if (this == &other) return *this;
    if (other.Ok()) {
      Assign(std::move(other.data_));
    } else {
      AssignStatus(std::move(other.status_));
    }
    return *this;
  }

  ~StatusOrData() {
    if (Ok()) {
      status_.~Status();
      data_.~T();
    } else {
      status_.~Status();
    }
  }

  template <class U>
  void Assign(U&& value) {
    if (Ok()) {
      data_ = std::forward<U>(value);
    } else {
      makeValue(std::forward<U>(value));
      status_ = Success();
    }
  }

  template <class U>
  void AssignStatus(U&& status) {
    clear();
    status_ = static_cast<Status>(std::forward<U>(status));
    ensureNotOk();
  }

  bool Ok() const { return status_.Ok(); }

 protected:
  // status_ will always be active after the constructor.
  // We make it a union to be able to initialize exactly how we need without
  // waste.
  // Eg. in the copy constructor we use the default constructor of Status in
  // the Ok() path to avoid an extra Ref call.
  union {
    Status status_;
  };

  // data_ is active iff status_.Ok()==true
  struct Dummy {};

  union {
    // When T is const, we need some non-const object we can cast to void* for
    // the placement new. dummy_ is that object.
    Dummy dummy_;
    T data_;
  };

  void clear() {
    if (Ok()) data_.~T();
  }

  void ensureOk() const {
    if (!Ok()) {
      throw VilaFatalException(
          CodeLocation(__FILE__, __LINE__, __FUNCTION__),
          "An error status is not a valid constructor argument to StatusOr<T>",
          CaptureBackTrace()
      );
    }
  }

  void ensureNotOk() const {
    if (Ok()) {
      throw VilaFatalException(
          CodeLocation(__FILE__, __LINE__, __FUNCTION__),
          "An OK status is not a valid constructor argument to StatusOr<T>",
          CaptureBackTrace()
      );
    }
  }

  // Construct the value (ie. data_) through placement new with the passed
  // argument.
  template <typename Arg>
  void makeValue(Arg&& arg) {
    internal_statusor::PlacementNew<T>(&dummy_, std::forward<Arg>(arg));
  }

  // Construct the status (ie. status_) through placement new with the passed
  // argument.
  template <typename... Args>
  void makeStatus(Args&&... args) {
    internal_statusor::PlacementNew<Status>(
        &status_, std::forward<Args>(args)...
    );
  }
};

// Helper base class to allow implicitly deleted constructors and assignment
// operations in StatusOr.
// TraitsBase will explicitly delete what it can't support and StatusOr will
// inherit that behavior implicitly.
template <bool Copy, bool Move>
struct TraitsBase {
  TraitsBase() = default;
  TraitsBase(const TraitsBase&) = default;
  TraitsBase(TraitsBase&&) noexcept = default;
  TraitsBase& operator=(const TraitsBase&) = default;
  TraitsBase& operator=(TraitsBase&&) noexcept = default;
};

template <>
struct TraitsBase<false, true> {
  TraitsBase() = default;
  TraitsBase(const TraitsBase&) = delete;
  TraitsBase(TraitsBase&&) noexcept = default;
  TraitsBase& operator=(const TraitsBase&) = delete;
  TraitsBase& operator=(TraitsBase&&) noexcept = default;
};

template <>
struct TraitsBase<false, false> {
  TraitsBase() = default;
  TraitsBase(const TraitsBase&) = delete;
  TraitsBase(TraitsBase&&) = delete;
  TraitsBase& operator=(const TraitsBase&) = delete;
  TraitsBase& operator=(TraitsBase&&) = delete;
};

}  // namespace internal_statusor
}  // namespace vila

#endif  // VILA_STATUS_STATUSOR_INTERNALS_H_
