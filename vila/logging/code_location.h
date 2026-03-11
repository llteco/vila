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
 * Description: Define the location in code
 ****************************************/
#ifndef VILA_LOGGING_CODE_LOCATION_H_
#define VILA_LOGGING_CODE_LOCATION_H_
#include <string>
#include <string_view>
#include <vector>

#include "fmt/format.h"

namespace vila {
/**
 * @brief Captures information on where in the source code a
 * message came from.
 */
class CodeLocation {
 public:
  /**
     @param file Usually the value of __FILE__
     @param line Usually the value of __LINE__
     @param func Usually the value of __PRETTY_FUNCTION__ or __FUNCTION__
  */
  constexpr CodeLocation(
      std::string_view file, const int line, std::string_view func
  )
      : file_path_{file}, line_num_{line}, func_{func} {}

  /** @brief Serialize to human-readable string. */
  std::string ToString() const {
    return fmt::format("{:s} ({:d}) {}", file_path_, line_num_, func_);
  }

 private:
  const std::string_view file_path_;
  const int line_num_;
  const std::string_view func_;
};

/**
 * @brief Capture backtrace and return function lists.
 *
 * @return function lists of names.
 */
std::vector<std::string> CaptureBackTrace();
}  // namespace vila

/**
 * @brief Formatter in {fmt} for type `CodeLocation`
 *
 * This is a general template formatter for type `CodeLocation`, inherited
 * from `basic_string_view<Char>`, so the format specifications such as {:}
 * follows the rules of basic string.
 * One method `format` must be implemented.
 * @note See https://fmt.dev/9.0.0/api.html#format-api
 * @example
 * fmt::format("{}", CodeLocation("a.c", "1", "void"));  // "a.c (1) void"
 */
template <typename Char>
struct fmt::formatter<vila::CodeLocation, Char>
    : fmt::formatter<fmt::basic_string_view<Char>> {
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-*) ignore function naming style
  auto format(const vila::CodeLocation& value, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), value.ToString());
  }
};
#endif  // VILA_LOGGING_CODE_LOCATION_H_
