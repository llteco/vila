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
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;

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
  static void LogMessage(int level, std::string_view tag, Args&&... args) {
    if (ShouldLog(level)) {
      tag.empty()
          ? Get()->Log(level, fmt::format(std::forward<Args>(args)...))
          : Get()->Log(level, tag, fmt::format(std::forward<Args>(args)...));
    }
  }

  virtual void Log(int level, std::string_view message) = 0;
  virtual void Log(
      int level, std::string_view tag, std::string_view message
  ) = 0;
#ifdef _WIN32
  virtual void Log(int level, std::wstring_view message) = 0;
  virtual void Log(
      int level, std::string_view tag, std::wstring_view message
  ) = 0;
#endif
};

template <class... Args>
inline void Logt(std::string_view tag, Args&&... args) {
  Logger::LogMessage(Logger::LOG_LEVEL_TRACE, tag, std::forward<Args>(args)...);
}

template <class... Args>
inline void Logd(std::string_view tag, Args&&... args) {
  Logger::LogMessage(Logger::LOG_LEVEL_DEBUG, tag, std::forward<Args>(args)...);
}

template <class... Args>
inline void Logi(std::string_view tag, Args&&... args) {
  Logger::LogMessage(Logger::LOG_LEVEL_INFO, tag, std::forward<Args>(args)...);
}

template <class... Args>
inline void Logw(std::string_view tag, Args&&... args) {
  Logger::LogMessage(Logger::LOG_LEVEL_WARN, tag, std::forward<Args>(args)...);
}

template <class... Args>
inline void Loge(std::string_view tag, Args&&... args) {
  Logger::LogMessage(Logger::LOG_LEVEL_ERROR, tag, std::forward<Args>(args)...);
}
}  // namespace vila

#define LOGT(...)  ::vila::Logt("", __VA_ARGS__)
#define LOGD(...)  ::vila::Logd("", __VA_ARGS__)
#define LOGI(...)  ::vila::Logi("", __VA_ARGS__)
#define LOGW(...)  ::vila::Logw("", __VA_ARGS__)
#define LOGE(...)  ::vila::Loge("", __VA_ARGS__)
#define VLOGT(...) ::vila::Logt(FILESTEM(__FILE__), __VA_ARGS__)
#define VLOGD(...) ::vila::Logd(FILESTEM(__FILE__), __VA_ARGS__)
#define VLOGI(...) ::vila::Logi(FILESTEM(__FILE__), __VA_ARGS__)
#define VLOGW(...) ::vila::Logw(FILESTEM(__FILE__), __VA_ARGS__)
#define VLOGE(...) ::vila::Loge(FILESTEM(__FILE__), __VA_ARGS__)
#endif  // VILA_LOGGING_LOGGER_H_
