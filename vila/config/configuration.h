/*
 * Copyright (C) 2023-2026 The VILA Authors.
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
