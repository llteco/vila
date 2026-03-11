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
/****************************************
 * Description: os utilities
 ****************************************/
#ifndef CLIM_OS_H_
#define CLIM_OS_H_
#include <map>
#include <string>

/**
 * @brief Get all system environment variables
 *
 * @return std::map<std::string, std::string>
 */
static std::map<std::string, std::string> Environ();

#ifdef _WIN32
#include <Windows.h>

static std::map<std::string, std::string> Environ() {
  std::map<std::string, std::string> ret;

  auto* envs = GetEnvironmentStrings();
  std::string k, v;  // key, value
  bool eos = false;  // end of string
  bool eok = false;
  for (int i = 0;; i++) {
    // end of whole string
    if (envs[i] == '\0' && eos) {
      break;
    } else {
      eos = false;
    }
    if (envs[i] == '\0') {
      eos = true;
      eok = false;
      if (!k.empty()) ret[k] = v;
      k.clear();
      v.clear();
    } else if (envs[i] == '=') {
      eok = true;
    } else if (eok) {
      v.push_back(static_cast<const char>(envs[i]));
    } else {
      k.push_back(static_cast<const char>(envs[i]));
    }
  }
  FreeEnvironmentStrings(envs);
  return ret;
}
#else
#include <unistd.h>

extern char** environ;  // NOLINT

static std::map<std::string, std::string> Environ() {
  std::map<std::string, std::string> ret;

  const char* envs = environ[0];
  std::string k, v;  // key, value
  bool eos = false;  // end of string
  bool eok = false;
  for (int i = 0;; i++) {
    // end of whole string
    if (envs[i] == '\0' && eos) {
      break;
    } else {
      eos = false;
    }
    if (envs[i] == '\0') {
      eos = true;
      eok = false;
      if (!k.empty()) ret[k] = v;
      k.clear();
      v.clear();
    } else if (envs[i] == '=') {
      eok = true;
    } else if (eok) {
      v.push_back(static_cast<const char>(envs[i]));
    } else {
      k.push_back(static_cast<const char>(envs[i]));
    }
  }
  return ret;
}
#endif
#endif  // CLIM_OS_H_
