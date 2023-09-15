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
#include "vila/widget/registration_token.h"

#include <utility>

namespace vila {
RegistrationToken::RegistrationToken(std::function<void()> unregisterer)
    : unregister_function_(std::move(unregisterer)) {}

RegistrationToken::RegistrationToken(RegistrationToken&& rhs) noexcept
    : unregister_function_(std::move(rhs.unregister_function_)) {
  rhs.unregister_function_ = nullptr;
}

RegistrationToken& RegistrationToken::operator=(RegistrationToken&& rhs
) noexcept {
  if (&rhs != this) {
    unregister_function_ = std::move(rhs.unregister_function_);
    rhs.unregister_function_ = nullptr;
  }
  return *this;
}

void RegistrationToken::Unregister() {
  if (unregister_function_ != nullptr) {
    unregister_function_();
    unregister_function_ = nullptr;
  }
}

namespace {
struct CombinedToken {
  void operator()() {
    for (auto& f : functions) {
      f();
    }
  }

  std::vector<std::function<void()>> functions;
};
}  // anonymous namespace

// static
RegistrationToken RegistrationToken::Combine(
    std::vector<RegistrationToken> tokens
) {
  CombinedToken combined;

  // When vector grows, it only moves elements if the move constructor is marked
  // noexcept (or if the element isn't copyable). In related news, function's
  // move constructor is not marked noexcept. By reserving the correct amount of
  // space up front, we remove the need for the vector to grow, and thus
  // eliminate copies.
  combined.functions.reserve(tokens.size());
  for (RegistrationToken& token : tokens) {
    combined.functions.push_back(std::move(token.unregister_function_));
  }
  return RegistrationToken(std::move(combined));
}

Unregister::Unregister(RegistrationToken token) : token_(std::move(token)) {}

Unregister::~Unregister() { token_.Unregister(); }

Unregister::Unregister(Unregister&& rhs) noexcept
    : token_(std::move(rhs.token_)) {}

Unregister& Unregister::operator=(Unregister&& rhs) noexcept {
  if (&rhs != this) {
    token_.Unregister();
    token_ = std::move(rhs.token_);
  }
  return *this;
}

void Unregister::Reset(RegistrationToken token) {
  token_.Unregister();
  token_ = std::move(token);
}
}  // namespace vila
