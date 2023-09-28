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
 * Description: Log system
 ****************************************/
#include "vila/logging/logger.h"

#include <atomic>
#include <cstdarg>
#include <fstream>
#include <memory>
#include <mutex>
#include <vector>

// Define your own level names as below
/* #define SPDLOG_LEVEL_NAMES \
  {"trace", "debug", "info", "warning", "error", "critical", "off"}
*/

#include "clim/os.h"
#include "clim/os_path.h"
#include "fmt/format.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
// Reject cases like using clang on windows
#if defined(ENABLE_WPP) && !defined(__clang__)
#include "vila/logging/logger.tmh"

/**
 * @brief Spdlog custom sink using windows WPP
 */
// NOLINTNEXTLINE: align with spdlog sink
class wpp_sink : public spdlog::sinks::base_sink<std::mutex> {
 public:
  wpp_sink() { WPP_INIT_TRACING(L"vila"); }

  ~wpp_sink() { WPP_CLEANUP(); }

 protected:
  void sink_it_(const spdlog::details::log_msg& msg) override {
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
    std::string msg_str = fmt::to_string(formatted);
    msg_str.pop_back();
    msg_str.pop_back();  // pop CRLF
    DoTraceMessage(Noise, "%s", msg_str.c_str());
  }

  void flush_() override {}
};
#endif

namespace vila {
class LoggerImpl final : public Logger {
 public:
  LoggerImpl();
  explicit LoggerImpl(std::string_view name);
  ~LoggerImpl();
  void Log(int level, std::string_view msg) override;
#ifdef _WIN32
  void Log(int level, std::wstring_view msg) override;
#endif
  void Flush() {
    if (logger_) {
      logger_->flush();
    }
  }

  LoggerImpl Clone(std::string_view name) {
    LoggerImpl nested;
    nested.name_ = fmt::format("{}.{}", name_, name);
    nested.logger_ = logger_->clone(nested.name_);
    if (!spdlog::get(nested.name_)) {
      spdlog::register_logger(nested.logger_);
    }
    return nested;
  }

 private:
  std::string name_;
  std::shared_ptr<spdlog::logger> logger_;
};

LoggerImpl::LoggerImpl() : LoggerImpl("vila") {}

LoggerImpl::LoggerImpl(std::string_view name)
    : name_(name), logger_(spdlog::get(name.data())) {
  if (logger_) {
    return;
  }
  std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#if defined(ENABLE_WPP) && !defined(__clang__)
  sinks.push_back(std::make_shared<wpp_sink>());
  sinks.back()->set_pattern("[%l][%n] %v");
#endif
#ifdef _DEBUG
  std::filesystem::path url(std::filesystem::temp_directory_path());
  url /= "vila.log";
  sinks.push_back(
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(url.wstring(), true)
  );
#endif
  auto dsink = std::make_shared<spdlog::sinks::dist_sink_mt>(sinks);
  logger_ = std::make_shared<spdlog::logger>(name_, dsink);
  logger_->set_level(spdlog::level::trace);
  logger_->set_pattern("[%t][%D %T.%e][%l][%n] %v");
  spdlog::register_logger(logger_);
}

LoggerImpl::~LoggerImpl() { Flush(); }

void LoggerImpl::Log(int level, std::string_view msg) {
  if (logger_) {
    logger_->log(static_cast<spdlog::level::level_enum>(level), "{}", msg);
  }
}

#ifdef _WIN32
void LoggerImpl::Log(int level, std::wstring_view msg) {
  if (logger_) {
    logger_->log(static_cast<spdlog::level::level_enum>(level), L"{}", msg);
  }
}
#endif

/// Force to allocate the instance on startup, make sure the change on the env
/// can take effect to the first log.
namespace {
Logger* glogger = Logger::Get();
std::atomic_int glevel = Logger::LOG_LEVEL_INFO;
}  // namespace

template <class T>
class ProtectWeakPtr {
 public:
  explicit ProtectWeakPtr(std::shared_ptr<T> ptr) : destroy_(false), ptr_(ptr) {
    /// init level from sys environ
    if (Environ().count("VILA_LOG_DEBUG")) {
      int lvl = std::atoi(Environ()["VILA_LOG_DEBUG"].c_str());
      Logger::SetLoggerLevel(lvl);
    }
  }

  ~ProtectWeakPtr() {
    lock_.lock();
    destroy_ = true;
    lock_.unlock();
  }

  std::shared_ptr<T> Acquire() {
    std::lock_guard<std::mutex> locker(lock_);
    if (destroy_) {
      return nullptr;
    }
    return ptr_.lock();
  }

 private:
  std::mutex lock_;
  bool destroy_;
  std::weak_ptr<T> ptr_;
};

struct DummyLogger final : public Logger {
  static Logger* Get() { return &instance; }

  void Log(int /*level*/, std::string_view /*msg*/) override {}
#ifdef _WIN32
  void Log(int /*level*/, std::wstring_view /*msg*/) override {}
#endif
  void Flush() {}

  DummyLogger Clone() { return *this; }

  static DummyLogger instance;
};

DummyLogger DummyLogger::instance;

#define _Q(a) #a
#define Q(a)  _Q(a)

Logger* Logger::Get() {
#ifdef VILA_PROJECT_NAME
  static std::shared_ptr<Logger> logger(new LoggerImpl(Q(VILA_PROJECT_NAME)));
#else
  static std::shared_ptr<Logger> logger(new LoggerImpl());
#endif
  /// Use a weak_ptr to test if the logger has been destroyed.
  static ProtectWeakPtr<Logger> protect(logger);
  auto p = protect.Acquire();
  if (!p) {
    /// If p is nullptr, use a Dummy struct so that the caller won't need to
    /// check nullptr. This struct is safe to call because there's nothing
    /// in it except a v-table.
    return DummyLogger::Get();
  }
  return p.get();
}

#undef Q
#undef _Q

Logger* Logger::Nest(std::string_view name) {
  static std::unordered_map<std::string, LoggerImpl> nested_loggers;
  auto* parent = static_cast<LoggerImpl*>(this);
  if (!parent) return nullptr;
  if (nested_loggers.count(std::string(name))) {
    return &nested_loggers[std::string(name)];
  }
  nested_loggers[std::string(name)] = parent->Clone(name);
  return &nested_loggers[std::string(name)];
}

void Logger::Drop() { spdlog::drop_all(); }

void Logger::SetLoggerLevel(int level) { glevel.store(level); }

bool Logger::ShouldLog(int level) {
  return glevel.load(std::memory_order_relaxed) <= level;
}

void Logger::Flush() {
  LoggerImpl* logger = static_cast<LoggerImpl*>(Get());
  logger->Flush();
}
}  // namespace vila
