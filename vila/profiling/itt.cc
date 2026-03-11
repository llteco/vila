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
#include "vila/profiling/itt.h"

#include <string>
#include <unordered_map>

#include "itt.h"

#ifdef VILA_ITT_ENABLED
#include <ittnotify.h>
#endif

namespace vila::itt {
domain_t* DomainManager::CreateDomain(const char* name) {
#ifdef VILA_ITT_ENABLED
  static std::unordered_map<std::string, __itt_domain*> domains;
  if (domains.count(name) == 0) {
#ifdef _WIN32
    domains[name] = __itt_domain_createA(name);
#else
    domains[name] = __itt_domain_create(name);
#endif
  }
  return reinterpret_cast<domain_t*>(domains[name]);
#else
  return nullptr;
#endif
}

FrameScope::FrameScope(const char* domain) {
#ifdef VILA_ITT_ENABLED
  curr_domain_ = DomainManager::CreateDomain(domain);
  __itt_id id = __itt_id_make(this, 0);
  __itt_frame_begin_v3(static_cast<__itt_domain*>(curr_domain_), &id);
#endif
}

FrameScope::FrameScope(const domain_t* domain) {
#ifdef VILA_ITT_ENABLED
  curr_domain_ = const_cast<domain_t*>(domain);
  __itt_id id = __itt_id_make(this, 0);
  __itt_frame_begin_v3(static_cast<__itt_domain*>(curr_domain_), &id);
#endif
}

FrameScope::~FrameScope() {
#ifdef VILA_ITT_ENABLED
  __itt_id id = __itt_id_make(this, 0);
  __itt_frame_end_v3(static_cast<__itt_domain*>(curr_domain_), &id);
#endif
}
}  // namespace vila::itt
