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
