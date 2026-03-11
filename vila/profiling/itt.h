/*
 * Copyright (C) 2025-2026 The VILA Authors.
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
#ifndef VILA_PROFILING_ITT_H_
#define VILA_PROFILING_ITT_H_

namespace vila {
namespace itt {
struct domain_t {};

/**
 * @brief Helper class to create and hold itt domain by name.
 */
class DomainManager {
 public:
  DomainManager() = delete;

  /**
   * @brief Create an ITT Domain handle
   *
   * @param name: name of the domain to specify
   * @return void* a handle to the domain
   */
  static domain_t* CreateDomain(const char* name);
};

/**
 * @brief Helper class to measure a scope of frame
 */
class FrameScope {
 public:
  /**
   * @brief Begin of a frame by name "domain"
   *
   * @param domain: name of the frame domain
   */
  FrameScope(const char* domain);

  /**
   * @brief Begin of a frame by domain handle
   *
   * @param domain: handle of the frame domain
   */
  FrameScope(const domain_t* domain);

  /**
   * @brief End of a frame
   */
  ~FrameScope();

 private:
  void* curr_domain_ = nullptr;
};
}  // namespace itt
}  // namespace vila

#endif /* VILA_PROFILING_ITT_H_ */
