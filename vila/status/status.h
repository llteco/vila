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
 * Description: Define status code
 ****************************************/
#ifndef VILA_STATUS_STATUS_H_
#define VILA_STATUS_STATUS_H_
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "fmt/format.h"
#include "fmt/ostream.h"
#include "vila/logging/code_location.h"
#include "vila/logging/logger.h"

namespace vila {
/**
 * @brief Define the friendly error code representation
 * @note all the entry should be manually assign a value,
 *       usually a warning value is positive and an error
 *       value is negative.
 */
enum class ErrorCode : int {
  /// an warning is not an error
  warning = 1,
  /// no error and no warnings
  success = 0,
  /// not specified error
  unknown = -1,
  /// not a valid config document
  invalid_configuration = -2,
  /// not valid parameters
  invalid_arguments = -3,
  /// object not found in registry
  object_not_found = -4,
  /// type not matched or unsupported
  type_error = -5,
  /// operation not supported
  unsupported = -6,
  /// memory allocate fail
  bad_alloc = -7,
  /// calculator open failed
  calculator_init_failed = -8,
  /// calculator fail to run or exit
  calculator_aborted = -9,
  /// waitable object timeout
  wait_time_out = -10,
  /// invalid call stack
  invalid_call = -11,
  /// skip all the next calculator
  calculator_skip = -12,
  /// null pointer error
  null_pointer = -13,
  /// not implemented
  not_implemented = -14,
  /// d3d errors
  d3d_internal_error = -15,
  /// opencl errors
  ocl_internal_error = -16,
  /// level-zero errors
  ze_internal_error = -17,
  /// VPE errors
  vpe_internal_error = -18,
  /// invalid memory shape
  invalid_memory_shape = -19,
  /// invalid memory layout
  invalid_memory_layout = -20,
  /// invalid memory format
  invalid_memory_format = -21,
  /// device context error
  context_changed = -30,
  /// internal error
  internal_error = -99,
};

/** @brief Returns the name for the status code. */
std::string StatusCodeToString(ErrorCode code);

/** @brief support format output on ostream and fmt. */
std::ostream& operator<<(std::ostream& os, ErrorCode code);

/**
 * @brief A status class representing message and return code.
 */
class Status final {
 public:
  Status();
  explicit Status(bool);
  explicit Status(int);
  Status(ErrorCode code, std::string_view msg);
  Status(const Status&) = default;
  Status& operator=(const Status& rhs) = default;
  Status(Status&&) noexcept;
  Status& operator=(Status&&) noexcept;

  ~Status();

  /// a successful state
  bool Ok() const;

  /// an error state, but not a warning
  bool Fatal() const;

  ErrorCode Code() const;

  int RawCode() const;

  std::string ToString() const;

  friend std::ostream& operator<<(std::ostream& os, const Status& s) {
    if (s.rep_) {
      return os << s.code_ << ": " << *s.rep_;
    }
    return os << s.code_;
  }

  bool operator==(const ErrorCode& code) const { return code_ == code; }

  friend Status ToWarning(const Status& sts);

 private:
  ErrorCode code_;
  std::shared_ptr<std::string> rep_;
};

/**
 * @brief Convert an error status to warning status.
 *
 * @param sts: any error states, if is success, do nothing.
 * @return Status, with positive error code.
 */
Status ToWarning(const Status& sts);

// Convenient status constructor
inline Status Success() { return {}; }

template <class T, class... Args>
inline Status Unknown(T fmt, Args&&... args) {
  return Status(
      ErrorCode::unknown, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidConfiguration(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_configuration,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidArguments(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_arguments,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status ObjectNotFound(T fmt, Args&&... args) {
  return Status(
      ErrorCode::object_not_found, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status TypeError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::type_error, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status Unsupported(T fmt, Args&&... args) {
  return Status(
      ErrorCode::unsupported, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status BadAlloc(T fmt, Args&&... args) {
  return Status(
      ErrorCode::bad_alloc, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status CalculatorInitFailed(T fmt, Args&&... args) {
  return Status(
      ErrorCode::calculator_init_failed,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status CalculatorAborted(T fmt, Args&&... args) {
  return Status(
      ErrorCode::calculator_aborted,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status WaitTimeout(T fmt, Args&&... args) {
  return Status(
      ErrorCode::wait_time_out, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidCall(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_call, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status CalculatorSkip(T fmt, Args&&... args) {
  return Status(
      ErrorCode::calculator_skip, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status NullPointer(T fmt, Args&&... args) {
  return Status(
      ErrorCode::null_pointer, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status NotImplemented(T fmt, Args&&... args) {
  return Status(
      ErrorCode::not_implemented, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status D3dInternalError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::d3d_internal_error,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status OclInternalError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::ocl_internal_error,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status ZeInternalError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::ze_internal_error,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status VpeInternalError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::vpe_internal_error,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidShape(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_memory_shape,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidLayout(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_memory_layout,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InvalidFormat(T fmt, Args&&... args) {
  return Status(
      ErrorCode::invalid_memory_format,
      fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status ContextChanged(T fmt, Args&&... args) {
  return Status(
      ErrorCode::context_changed, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

template <class T, class... Args>
inline Status InternalError(T fmt, Args&&... args) {
  return Status(
      ErrorCode::internal_error, fmt::format(fmt, std::forward<Args>(args)...)
  );
}

/**
 * @brief This exception is used to throw iff a fatal error occurs.
 * Do not throw it in the common logic.
 */
class VilaFatalException : public std::exception {
 public:
  /**
   * @brief Create a new exception that captures the location it was thrown
   *   from.
   * @param location Location in the source code the exception is being thrown
   *   from.
   * @param msg Message containing additional information about the exception
   *   cause.
   * @param bt: backtraces
   */
  VilaFatalException(
      const CodeLocation& location,
      std::string_view msg,
      const std::vector<std::string>& bt
  );

  /** @brief the C++ exception API. */
  const char* what() const noexcept override { return what_.c_str(); }

 private:
  std::string what_;
};

#include "status_check.inl"  // NOLINT
}  // namespace vila

/// @brief formatter of Status, using ostream overload function
template <>
struct fmt::formatter<vila::Status> : fmt::ostream_formatter {};

/// @brief formatter of ErrorCode, using ostream overload function
template <>
struct fmt::formatter<vila::ErrorCode> : fmt::ostream_formatter {};
#endif  // VILA_STATUS_STATUS_H_
