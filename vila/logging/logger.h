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
#ifndef VILA_LOGGING_LOGGER_H_
#define VILA_LOGGING_LOGGER_H_
#include <string>
#include <string_view>
#include <utility>

#include "clim/reflect.h"
#include "fmt/format.h"
#include "fmt/xchar.h"

namespace vila {
/**
 * @brief A unified named log system
 *
 * Set level <0 to flush into file
 */
class Logger {
 public:
  Logger() = default;

  enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
  };

  /**
   * @brief Get the singleton Logger.
   */
  static Logger* Get();

  /**
   * @brief Get a logger with nested name
   *
   * @param name: a name for the nested logger
   */
  Logger* Nest(std::string_view name);

  /**
   * @brief Drop the logger.
   * Calling any log function after dropping logger is UB, and may crash.
   */
  static void Drop();

  /**
   * @brief Set the verbose level to the logger
   *
   * @param level : Use positive value to log to console, negative value to file
   */
  static void SetLoggerLevel(int level);

  /**
   * @brief Whether can be logging at the given level
   * @param [in] level: logging level.
   *
   * @return true: the current log level is above the throttle
   * @return false: the current log level is below the throttle
   */
  static bool ShouldLog(int level);

  /**
   * @brief Flush buffered content to sink
   */
  static void Flush();

  template <class... Args>
  void Trace(Args&&... args) {
    if (ShouldLog(LOG_LEVEL_TRACE)) {
      Log(LOG_LEVEL_TRACE, fmt::format(std::forward<Args>(args)...));
    }
  }

  template <class... Args>
  void Debug(Args&&... args) {
    if (ShouldLog(LOG_LEVEL_DEBUG)) {
      Log(LOG_LEVEL_DEBUG, fmt::format(std::forward<Args>(args)...));
    }
  }

  template <class... Args>
  void Info(Args&&... args) {
    if (ShouldLog(LOG_LEVEL_INFO)) {
      Log(LOG_LEVEL_INFO, fmt::format(std::forward<Args>(args)...));
    }
  }

  template <class... Args>
  void Warning(Args&&... args) {
    if (ShouldLog(LOG_LEVEL_WARN)) {
      Log(LOG_LEVEL_WARN, fmt::format(std::forward<Args>(args)...));
    }
  }

  template <class... Args>
  void Error(Args&&... args) {
    if (ShouldLog(LOG_LEVEL_ERROR)) {
      Log(LOG_LEVEL_ERROR, fmt::format(std::forward<Args>(args)...));
    }
  }

  virtual void Log(int level, std::string_view message) = 0;
#ifdef _WIN32
  virtual void Log(int level, std::wstring_view message) = 0;
#endif
};
}  // namespace vila

#ifdef VILA_USE_LOG
#define LOGT(...) ::vila::Logger::Get()->Trace(__VA_ARGS__)
#define LOGD(...) ::vila::Logger::Get()->Debug(__VA_ARGS__)
#define LOGI(...) ::vila::Logger::Get()->Info(__VA_ARGS__)
#define LOGW(...) ::vila::Logger::Get()->Warning(__VA_ARGS__)
#define LOGE(...) ::vila::Logger::Get()->Error(__VA_ARGS__)
#define VLOGT(...) \
  ::vila::Logger::Get()->Nest(FILESTEM(__FILE__))->Trace(__VA_ARGS__)
#define VLOGD(...) \
  ::vila::Logger::Get()->Nest(FILESTEM(__FILE__))->Debug(__VA_ARGS__)
#define VLOGI(...) \
  ::vila::Logger::Get()->Nest(FILESTEM(__FILE__))->Info(__VA_ARGS__)
#define VLOGW(...) \
  ::vila::Logger::Get()->Nest(FILESTEM(__FILE__))->Warning(__VA_ARGS__)
#define VLOGE(...) \
  ::vila::Logger::Get()->Nest(FILESTEM(__FILE__))->Error(__VA_ARGS__)
#else
#define LOGT(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define VLOGT(...)
#define VLOGD(...)
#define VLOGI(...)
#define VLOGW(...)
#define VLOGE(...)
#endif
#endif  // VILA_LOGGING_LOGGER_H_
