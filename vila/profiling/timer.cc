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
#include "vila/profiling/timer.h"

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "clim/numerical.h"
#include "clim/vt/vt.h"
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
  void PrintAnalyze() const;
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
    events_[tag].push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(
                               endpoint - tp_[tag]
    )
                               .count());
    tp_.erase(tag);
  }
}

void Profiler::ProfilerImpl::AddProfile(
    const std::string& tag, std::chrono::nanoseconds ns
) {
  std::lock_guard<std::mutex> locker(mutex_);
  events_[tag].push_back(ns.count());
}

void Profiler::ProfilerImpl::PrintAnalyze() const {
  for (const auto& event : events_) {
    const auto& ds = event.second;
    auto avg = vt::ReduceMean(ds) / 1e6;
    auto std = sqrt(vt::ReduceVar(ds)) / 1e6;
    auto longest = numeric_div<double>(vt::ReduceMax(ds), 1e6);
    auto shortest = numeric_div<double>(vt::ReduceMin(ds), 1e6);
    LOGI(
        "{}: AVG={:.2f} STD={:.2} MAX={:.2f} MIN={:.2f} CALL={}", event.first,
        avg, std, longest, shortest, ds.size()
    );
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

void Profiler::PrintAnalyze() const { p_impl_->PrintAnalyze(); }

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
