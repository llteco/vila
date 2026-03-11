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
 * Description: tracing functions
 ****************************************/
#ifndef VILA_PROFILING_TRACE_H_
#define VILA_PROFILING_TRACE_H_
#include <array>
#include <memory>
#include <utility>
#include <vector>

#include "vila/logging/logger.h"

namespace vila {
/**
 * @brief Add function entry and exit log automatically.
 *
 * Use with macro DEBUGTRACE.
 */
struct AutoTracer {
  explicit AutoTracer(std::string_view func) : func_(func) {
    LOGT("{} +", func_);
  }

  ~AutoTracer() { LOGT("{} -", func_); }

  const std::string_view func_;
};

/**
 * @brief A binary tracer to log binary data with given shape.
 *
 * Set environment variable `VILA_TRACE_DEBUG=N`, where N is an integer
 * represents the start frame of tracing. Set environment variable
 * `VILA_TRACE_DEBUG=N,M`, where M is an integer represents the end frame of
 * tracing.
 */
class BinTracer {
 public:
  using blob = std::vector<size_t>;
  using char3 = std::array<char, 3>;

  virtual ~BinTracer() = default;

  static BinTracer& Instance();

  /// @brief Increase the number of frame counter, each trace log contains a
  /// frame id.
  virtual void StepFrameId() = 0;

  /// @brief Whether the current frame is in the range of tracing [start, end].
  virtual bool ShouldTrace() const = 0;

  virtual void Trace(std::string_view tag, char* bin, const blob& shape) = 0;
  virtual void Trace(std::string_view tag, char3* bin, const blob& shape) = 0;
  virtual void Trace(std::string_view tag, int* bin, const blob& shape) = 0;
  virtual void Trace(std::string_view tag, float* bin, const blob& shape) = 0;
  virtual void Trace(std::string_view tag, double* bin, const blob& shape) = 0;

  /**
   * @brief Trace a binary data with given shape.
   *
   * @tparam T: type of data
   * @param tag: name of data, better to use node id with port name
   * @param bin: start pointer of the data
   * @param size: a list of data shape
   */
  template <class T, class... Shape>
  void Trace(std::string_view tag, T* bin, Shape... size) {
    blob shape{std::forward<Shape>(size)...};
    Trace(tag, bin, shape);
  }
};

#ifdef _DEBUG
#define DEBUGTRACE() ::vila::AutoTracer t_##__LINE__(__FUNCTION__)
#else
#define DEBUGTRACE(...)
#endif
}  // namespace vila
#endif  // VILA_PROFILING_TRACE_H_
