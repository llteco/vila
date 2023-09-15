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
 ******************************************************************************/

#define CHECK_AND_RETURN(sts)                                          \
  do {                                                                 \
    ::vila::Status __ret(sts);                                         \
    auto loc = ::vila::CodeLocation(__FILE__, __LINE__, __FUNCTION__); \
    if (!__ret.Ok()) {                                                 \
      LOGT("{}", #sts);                                                \
      LOGE("{}\n @ {}", __ret, loc);                                   \
      return __ret;                                                    \
    } else {                                                           \
      LOGT("{} {}", #sts, loc);                                        \
    }                                                                  \
  } while (0)

#define ASSERT_OK(sts, ...)                                             \
  do {                                                                  \
    ::vila::Status __ret(sts);                                          \
    if (!__ret.Ok()) {                                                  \
      LOGE("{}", __ret);                                                \
      throw ::vila::VilaFatalException(                                 \
          ::vila::CodeLocation(__FILE__, __LINE__, __FUNCTION__), #sts, \
          ::vila::CaptureBackTrace()                                    \
      );                                                                \
    }                                                                   \
  } while (0)
