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

enum class ProfileFormat {
  text,
  json,
};

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

  void PrintAnalyze(ProfileFormat format = ProfileFormat::text) const;

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
