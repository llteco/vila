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
#include "vila/profiling/timer.h"

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "clim/numerical.h"
#include "clim/vt/vt.h"
#include "vila/config/json.hpp"
#include "vila/logging/logger.h"

#ifdef _WIN32
#include <Windows.h>
#include <timeapi.h>
#endif
namespace vila {
using TimePoint = std::chrono::high_resolution_clock::time_point;

struct Profiler::ProfilerImpl {
  ProfilerImpl() = default;

  ~ProfilerImpl() = default;

  void StartProfile(const std::string& tag);
  void EndProfile(const std::string& tag);
  void AddProfile(const std::string& tag, std::chrono::nanoseconds ns);
  void PrintAnalyze(ProfileFormat format) const;
  TimePoint StartTime();

  std::mutex mutex_;
  std::unordered_map<std::string, TimePoint> tp_;
  std::map<std::string, std::vector<uint64_t>> events_;
};

TimePoint Profiler::ProfilerImpl::StartTime() {
  return std::chrono::high_resolution_clock::now();
}

void Profiler::ProfilerImpl::StartProfile(const std::string& tag) {
  std::lock_guard<std::mutex> locker(mutex_);
  tp_[std::string(tag)] = StartTime();
}

void Profiler::ProfilerImpl::EndProfile(const std::string& tag) {
  std::lock_guard<std::mutex> locker(mutex_);
  if (tp_.count(tag) != 0) {
    auto endpoint = std::chrono::high_resolution_clock::now();
    events_[tag].push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            endpoint - tp_[tag]
        )
            .count()
    );
    tp_.erase(tag);
  }
}

void Profiler::ProfilerImpl::AddProfile(
    const std::string& tag, std::chrono::nanoseconds ns
) {
  std::lock_guard<std::mutex> locker(mutex_);
  events_[tag].push_back(ns.count());
}

void Profiler::ProfilerImpl::PrintAnalyze(ProfileFormat format) const {
  nlohmann::json results;
  for (const auto& event : events_) {
    const auto& ds = event.second;
    auto avg = vt::ReduceMean(ds) / 1e6;
    auto std = sqrt(vt::ReduceVar(ds)) / 1e6;
    auto med = numeric_div<double>(vt::Median(ds), 1e6);
    auto longest = numeric_div<double>(vt::ReduceMax(ds), 1e6);
    auto shortest = numeric_div<double>(vt::ReduceMin(ds), 1e6);
    if (format == ProfileFormat::text) {
      fmt::println(
          "{}: MED={:.2f} AVG={:.2f} STD={:.2} MAX={:.2f} MIN={:.2f} CALL={}",
          event.first, med, avg, std, longest, shortest, ds.size()
      );
    } else if (format == ProfileFormat::json) {
      nlohmann::json item;
      item["name"] = event.first;
      item["med"] = med;
      item["avg"] = avg;
      item["std"] = std;
      item["max"] = longest;
      item["min"] = shortest;
      item["call"] = ds.size();
      results.push_back(item);
    }
  }
  if (format == ProfileFormat::json) {
    fmt::println("{}", results.dump(4));
  }
}

Profiler* Profiler::Instance() {
  static Profiler instance;
  return &instance;
}

Profiler::Profiler() { p_impl_ = std::make_unique<Profiler::ProfilerImpl>(); }

Profiler::~Profiler() = default;

void Profiler::StartProfile(std::string_view tag) {
  p_impl_->StartProfile(std::string(tag));
}

void Profiler::EndProfile(std::string_view tag) {
  p_impl_->EndProfile(std::string(tag));
}

std::vector<uint64_t> Profiler::GetProfile(std::string_view tag) const {
  return p_impl_->events_.at(std::string(tag));
}

void Profiler::AddProfile(std::string_view tag, std::chrono::nanoseconds ns) {
  p_impl_->AddProfile(std::string(tag), ns);
}

void Profiler::PrintAnalyze(ProfileFormat format) const {
  p_impl_->PrintAnalyze(format);
}

#ifdef _WIN32
/**
 * @brief Changing timer period for more accurate calculating
 *        Only supported on Windows OS
 */
static int timer_min_period = 0;

struct SetTimerPeriod::SetTimerPeriodImpl {
  SetTimerPeriodImpl() = default;

  ~SetTimerPeriodImpl() = default;

  void Start();
  void End();
};

void SetTimerPeriod::Start() { p_impl_->Start(); }

void SetTimerPeriod::End() { p_impl_->End(); }

void SetTimerPeriod::SetTimerPeriodImpl::Start() {
  if (!timer_min_period) {
    // Get minimal timer period for 1st time.
    TIMECAPS tc;
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
      timer_min_period = tc.wPeriodMin;
    }
  } else {
    // After get minimal time period,
    // set minimal timer period.
    timeBeginPeriod(timer_min_period);
  }
}

void SetTimerPeriod::SetTimerPeriodImpl::End() {
  if (timer_min_period) {
    // Don't forget to clear minimal timer period
    // after each time used.
    timeEndPeriod(timer_min_period);
  }
}

SetTimerPeriod* SetTimerPeriod::Instance() {
  static SetTimerPeriod instance;
  return &instance;
}

SetTimerPeriod::SetTimerPeriod() {
  p_impl_ = std::make_unique<SetTimerPeriod::SetTimerPeriodImpl>();
}
#endif
}  // namespace vila
