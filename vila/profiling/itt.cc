/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2025 Intel Corporation. All Rights Reserved.
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
 ******************************************************************************/
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
