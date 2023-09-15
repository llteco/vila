/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
/****************************************
 * Description: Profiling timer
 ****************************************/
#ifndef VILA_PROFILING_TIMER_H_
#define VILA_PROFILING_TIMER_H_
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vila {
/**
 * @brief Recording execution time (of CPU)
 */
class Profiler {
 public:
  Profiler(const Profiler&) = delete;
  Profiler& operator=(const Profiler&) = delete;
  ~Profiler();

  static Profiler* Instance();

  void StartProfile(std::string_view tag);

  void EndProfile(std::string_view tag);

  std::vector<uint64_t> GetProfile(std::string_view tag) const;

  void AddProfile(std::string_view tag, std::chrono::nanoseconds ns);

  void PrintAnalyze() const;

 private:
  Profiler();

  struct ProfilerImpl;
  std::unique_ptr<ProfilerImpl> p_impl_;
};

struct AutoProfiler {
  explicit AutoProfiler(std::string_view tag) : tag_(tag) {
    Profiler::Instance()->StartProfile(tag_);
  }

  ~AutoProfiler() { Profiler::Instance()->EndProfile(tag_); }

  const std::string tag_;
};

#ifdef _WIN32
/**
 * @brief Changing timer period for more accurate calculating
 */
class SetTimerPeriod {
 public:
  static SetTimerPeriod* Instance();
  void Start();
  void End();

 private:
  SetTimerPeriod();
  struct SetTimerPeriodImpl;
  std::unique_ptr<SetTimerPeriodImpl> p_impl_;
};

struct AutoSetTimerPeriod {
  AutoSetTimerPeriod() { SetTimerPeriod::Instance()->Start(); }

  ~AutoSetTimerPeriod() { SetTimerPeriod::Instance()->End(); }
};
#endif
}  // namespace vila
#endif  // VILA_PROFILING_TIMER_H_
