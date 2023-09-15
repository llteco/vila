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
#ifndef CLIM_TYPE_TRAITS_H_
#define CLIM_TYPE_TRAITS_H_
#include <type_traits>

template <class T, class Ty = int>
using must_be_int = std::enable_if_t<std::is_integral<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_float = std::enable_if_t<std::is_floating_point<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_number = std::enable_if_t<std::is_arithmetic<T>::value, Ty>;
template <class T, class Ty = int>
using must_be_pointer = std::enable_if_t<std::is_pointer<T>::value, Ty>;
template <class T, class Ty = int>
using must_not_be_pointer = std::enable_if_t<!std::is_pointer<T>::value, Ty>;
#endif  // CLIM_TYPE_TRAITS_H_
