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
