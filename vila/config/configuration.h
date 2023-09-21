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
#ifndef VILA_CONFIG_CONFIGURATION_H_
#define VILA_CONFIG_CONFIGURATION_H_
#include <sstream>
// clang-format off
#include "vila/config/json_fwd.hpp"
#include "vila/config/json.hpp"

// clang-format on

namespace vila {
using Json = nlohmann::json;

class Configuration {};

/**
 * @brief Read JSON from an std input stream.
 *
 * @param is: std input stream, discard the current position
 * @return json
 */
Json JsonFromStream(std::istream& is);

/**
 * @brief Read JSON from a json file.
 *
 * @param filename: filename
 * @return json
 */
Json JsonFromFile(const std::string& filename);

/**
 * @brief Read JSON from in-memory document.
 *
 * @param doc: a JSON document string
 * @return Json
 */
Json JsonFromDoc(const std::string& doc);
}  // namespace vila

#endif  // VILA_CONFIG_CONFIGURATION_H_
