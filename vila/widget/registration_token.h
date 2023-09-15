/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
#ifndef VILA_WIDGET_REGISTRATION_TOKEN_H_
#define VILA_WIDGET_REGISTRATION_TOKEN_H_
#include <functional>
#include <vector>

namespace vila {
class RegistrationToken {
 public:
  explicit RegistrationToken(std::function<void()> unregisterer);

  // It is useful to have an empty constructor for when we want to declare a
  // token, and assign it later.
  RegistrationToken() = default;

  RegistrationToken(const RegistrationToken&) = delete;
  RegistrationToken& operator=(const RegistrationToken&) = delete;

  RegistrationToken(RegistrationToken&& rhs) noexcept;
  RegistrationToken& operator=(RegistrationToken&& rhs) noexcept;

  // Unregisters the registration for which this token is in charge, and voids
  // the token. It is safe to call this more than once, but further calls are
  // guaranteed to be noop.
  void Unregister();

  // Returns a token whose Unregister() will Unregister() all <tokens>.
  static RegistrationToken Combine(std::vector<RegistrationToken> tokens);

 private:
  std::function<void()> unregister_function_ = nullptr;
};

// RAII class for registration tokens: it calls Unregister() when it goes out
// of scope.
class Unregister {
 public:
  // Useful to have an empty constructor for when we want to assign it later.
  // The default is an empty token that does nothing.
  Unregister() : token_() {}

  explicit Unregister(RegistrationToken token);
  ~Unregister();

  Unregister(const Unregister&) = delete;
  Unregister& operator=(const Unregister&) = delete;

  Unregister(Unregister&& rhs) noexcept;
  Unregister& operator=(Unregister&& rhs) noexcept;

  // Similar to unique_ptr.reset() and the likes: this will unregister the
  // current token if any, and then assume registration ownership of this new
  // <token>.
  void Reset(RegistrationToken token = RegistrationToken());

 private:
  RegistrationToken token_;
};
}  // namespace vila
#endif  // VILA_WIDGET_REGISTRATION_TOKEN_H_
