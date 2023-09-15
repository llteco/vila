/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2023 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 *
 * USAGE
 * ~~~~~
 * StatusOr<T> is the union of a Status object and a T
 * object. StatusOr models the concept of an object that is either a
 * usable value, or an error Status explaining why such a value is
 * not present. To this end, StatusOr<T> does not allow its Status
 * value to be Status::OK. Furthermore, the value of a StatusOr<T*>
 * must not be null. This is enforced by a debug check in most cases,
 * but even when it is not, clients must not set the value to null.
 *
 * The primary use-case for StatusOr<T> is as the return value of a
 * function which may fail.
 *
 * Example client usage for a StatusOr<T>, where T is not a pointer:
 *
 *  vila::StatusOr<float> result = DoBigCalculationThatCouldFail();
 *  if (result.Ok()) {
 *    float answer = result.ValueOrDie();
 *    printf("Big calculation yielded: %f", answer);
 *  } else {
 *    LOG(ERROR) << result.status();
 *  }
 *
 * Example client usage for a StatusOr<T*>:
 *
 *  vila::StatusOr<Foo*> result = FooFactory::MakeNewFoo(arg);
 *  if (result.Ok()) {
 *    std::unique_ptr<Foo> foo(result.ValueOrDie());
 *    foo->DoSomethingCool();
 *  } else {
 *    LOG(ERROR) << result.status();
 *  }
 *
 * Example client usage for a StatusOr<std::unique_ptr<T>>:
 *
 *  vila::StatusOr<std::unique_ptr<Foo>> result =
 *    FooFactory::MakeNewFoo(arg);
 *  if (result.Ok()) {
 *    std::unique_ptr<Foo> foo = std::move(result.ValueOrDie());
 *    foo->DoSomethingCool();
 *  } else {
 *    LOG(ERROR) << result.status();
 *  }
 *
 * Example factory implementation returning StatusOr<T*>:
 *
 *  vila::StatusOr<Foo*> FooFactory::MakeNewFoo(int arg) {
 *    if (arg <= 0) {
 *      return vila::InvalidArgumentError("Arg must be positive");
 *    } else {
 *      return new Foo(arg);
 *    }
 *  }
 *
 * Note that the assignment operators require that destroying the currently
 * stored value cannot invalidate the argument; in other words, the argument
 * cannot be an alias for the current value, or anything owned by the current
 * value.
 ******************************************************************************/
#ifndef VILA_STATUS_STATUSOR_H_
#define VILA_STATUS_STATUSOR_H_
#include <utility>

#include "vila/status/status.h"
#include "vila/status/statusor_internals.h"

namespace vila {

template <typename T>
class StatusOr : private internal_statusor::StatusOrData<T>,
                 private internal_statusor::TraitsBase<
                     std::is_copy_constructible<T>::value,
                     std::is_move_constructible<T>::value> {
  template <typename U>
  friend class StatusOr;

  using Base = internal_statusor::StatusOrData<T>;

 public:
  using element_type = T;

  // Constructs a new StatusOr with Status::UNKNOWN status.  This is marked
  // 'explicit' to try to catch cases like 'return {};', where people think
  // StatusOr<std::vector<int>> will be initialized with an empty vector,
  // instead of a Status::UNKNOWN status.
  explicit StatusOr();  // NOLINT

  // StatusOr<T> will be copy constructible/assignable if T is copy
  // constructible.
  StatusOr(const StatusOr&) = default;
  StatusOr& operator=(const StatusOr&) = default;

  // StatusOr<T> will be move constructible/assignable if T is move
  // constructible.
  StatusOr(StatusOr&&) noexcept = default;
  StatusOr& operator=(StatusOr&&) noexcept = default;

  // Conversion copy/move constructor, T must be convertible from U.
  // TODO(NOLINT): These should not participate in overload resolution if U
  // is not convertible to T.
  template <typename U>
  StatusOr(const StatusOr<U>& other);  // NOLINT(*-explicit-*)
  template <typename U>
  StatusOr(StatusOr<U>&& other);  // NOLINT(*-explicit-*)

  // Conversion copy/move assignment operator, T must be convertible from U.
  template <typename U>
  StatusOr& operator=(const StatusOr<U>& other);
  template <typename U>
  StatusOr& operator=(StatusOr<U>&& other);

  // Constructs a new StatusOr with the given value. After calling this
  // constructor, calls to ValueOrDie() will succeed, and calls to Status() will
  // return OK.
  //
  // NOTE: Not explicit - we want to use StatusOr<T> as a return type
  // so it is convenient and sensible to be able to do 'return T()'
  // when the return type is StatusOr<T>.
  //
  // REQUIRES: T is copy constructible.
  StatusOr(const T& value);  // NOLINT

  // Constructs a new StatusOr with the given non-ok status. After calling
  // this constructor, calls to ValueOrDie() will CHECK-fail.
  //
  // NOTE: Not explicit - we want to use StatusOr<T> as a return
  // value, so it is convenient and sensible to be able to do 'return
  // Status()' when the return type is StatusOr<T>.
  //
  // REQUIRES: !status.Ok(). This requirement is DCHECKed.
  // In optimized builds, passing Status::OK() here will have the effect
  // of passing vila::StatusCode::kInternal as a fallback.
  StatusOr(const Status& status);  // NOLINT
  StatusOr& operator=(const Status& status);

  // TODO(NOLINT): Add operator=(T) overloads.

  // Similar to the `const T&` overload.
  //
  // REQUIRES: T is move constructible.
  StatusOr(T&& value);  // NOLINT

  // RValue versions of the operations declared above.
  StatusOr(Status&& status);  // NOLINT
  StatusOr& operator=(Status&& status);

  // Returns this->Status().Ok()
  bool Ok() const { return this->status_.Ok(); }

  // Returns a reference to vila status. If this contains a T, then
  // returns Status::OK().
  const vila::Status& status() const&;  // NOLINT: conflict with class Status
  vila::Status status() &&;             // NOLINT: conflict with class Status

  // Returns a reference to our current value, or CHECK-fails if !this->Ok().
  //
  // Note: for value types that are cheap to copy, prefer simple code:
  //
  //   T value = statusor.ValueOrDie();
  //
  // Otherwise, if the value type is expensive to copy, but can be left
  // in the StatusOr, simply assign to a reference:
  //
  //   T& value = statusor.ValueOrDie();  // or `const T&`
  //
  // Otherwise, if the value type supports an efficient move, it can be
  // used as follows:
  //
  //   T value = std::move(statusor).ValueOrDie();
  //
  // The std::move on statusor instead of on the whole expression enables
  // warnings about possible uses of the statusor object after the move.
  // C++ style guide waiver for ref-qualified overloads granted in cl/143176389
  // See go/ref-qualifiers for more details on such overloads.
  const T& ValueOrDie() const&;
  T& ValueOrDie() &;
  const T&& ValueOrDie() const&&;
  T&& ValueOrDie() &&;

  T ConsumeValueOrDie() { return std::move(ValueOrDie()); }

  // Ignores any errors. This method does nothing except potentially suppress
  // complaints from any tools that are checking that errors are not dropped on
  // the floor.
  void IgnoreError() const;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation details for StatusOr<T>

template <typename T>
StatusOr<T>::StatusOr() : Base(Unknown("")) {}

template <typename T>
StatusOr<T>::StatusOr(const T& value) : Base(value) {}

template <typename T>
StatusOr<T>::StatusOr(const class Status& status) : Base(status) {}

template <typename T>
StatusOr<T>& StatusOr<T>::operator=(const class Status& status) {
  this->Assign(status);
  return *this;
}

template <typename T>
StatusOr<T>::StatusOr(T&& value) : Base(std::move(value)) {}

template <typename T>
StatusOr<T>::StatusOr(class Status&& status) : Base(std::move(status)) {}

template <typename T>
StatusOr<T>& StatusOr<T>::operator=(class Status&& status) {
  this->Assign(std::move(status));
  return *this;
}

template <typename T>
template <typename U>
inline StatusOr<T>::StatusOr(const StatusOr<U>& other)
    : Base(static_cast<const typename StatusOr<U>::Base&>(other)) {}

template <typename T>
template <typename U>
inline StatusOr<T>& StatusOr<T>::operator=(const StatusOr<U>& other) {
  if (other.Ok())
    this->Assign(other.ValueOrDie());
  else
    this->Assign(other.status());
  return *this;
}

template <typename T>
template <typename U>
inline StatusOr<T>::StatusOr(StatusOr<U>&& other)
    : Base(static_cast<typename StatusOr<U>::Base&&>(other)) {}

template <typename T>
template <typename U>
inline StatusOr<T>& StatusOr<T>::operator=(StatusOr<U>&& other) {
  if (other.Ok()) {
    this->Assign(std::move(other).ValueOrDie());
  } else {
    this->Assign(std::move(other).status());
  }
  return *this;
}

template <typename T>
const Status& StatusOr<T>::status() const& {
  return this->status_;
}

template <typename T>
Status StatusOr<T>::status() && {
  return Ok() ? Success() : std::move(this->status_);
}

template <typename T>
const T& StatusOr<T>::ValueOrDie() const& {
  this->ensureOk();
  return this->data_;
}

template <typename T>
T& StatusOr<T>::ValueOrDie() & {
  this->ensureOk();
  return this->data_;
}

template <typename T>
const T&& StatusOr<T>::ValueOrDie() const&& {
  this->ensureOk();
  return std::move(this->data_);
}

template <typename T>
T&& StatusOr<T>::ValueOrDie() && {
  this->ensureOk();
  return std::move(this->data_);
}

template <typename T>
void StatusOr<T>::IgnoreError() const {
  // no-op
}

}  // namespace vila

#endif  // VILA_STATUS_STATUSOR_H_
