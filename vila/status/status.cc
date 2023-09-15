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
#include "vila/status/status.h"

#include <algorithm>
#include <memory>

#include "vila/logging/logger.h"

namespace vila {
std::string StatusCodeToString(ErrorCode code) {
  switch (code) {
    case ErrorCode::warning:
      return "WARNING";
    case ErrorCode::success:
      return "SUCCESS";
    case ErrorCode::invalid_configuration:
      return "INVALID_CONFIGURATION";
    case ErrorCode::invalid_arguments:
      return "INVALID_ARGUMENTS";
    case ErrorCode::object_not_found:
      return "OBJECT_NOT_FOUND";
    case ErrorCode::type_error:
      return "TYPE_ERROR";
    case ErrorCode::unsupported:
      return "UNSUPPORTED";
    case ErrorCode::bad_alloc:
      return "BAD_ALLOC";
    case ErrorCode::calculator_init_failed:
      return "CALCULATOR_INIT_FAILED";
    case ErrorCode::calculator_aborted:
      return "CALCULATOR_ABORTED";
    case ErrorCode::wait_time_out:
      return "WAIT_TIME_OUT";
    case ErrorCode::invalid_call:
      return "INVALID_CALL";
    case ErrorCode::internal_error:
      return "INTERNAL_ERROR";
    case ErrorCode::calculator_skip:
      return "CALCULATOR_SKIP";
    case ErrorCode::null_pointer:
      return "NULL_POINTER";
    case ErrorCode::not_implemented:
      return "NOT_IMPLEMENTED";
    case ErrorCode::d3d_internal_error:
      return "D3D_INTERNAL_ERROR";
    case ErrorCode::ocl_internal_error:
      return "OCL_INTERNAL_ERROR";
    case ErrorCode::ze_internal_error:
      return "ZE_INTERNAL_ERROR";
    case ErrorCode::vpe_internal_error:
      return "VPE_INTERNAL_ERROR";
    case ErrorCode::invalid_memory_shape:
      return "INVALID_MEMORY_SHAPE";
    case ErrorCode::invalid_memory_layout:
      return "INVALID_MEMORY_LAYOUT";
    case ErrorCode::invalid_memory_format:
      return "INVALID_MEMORY_FORMAT";
    case ErrorCode::context_changed:
      return "CONTEXT_CHANGED";
    case ErrorCode::unknown:
    default:
      return "UNKNOWN";
  }
}

std::ostream& operator<<(std::ostream& os, ErrorCode code) {
  return os << StatusCodeToString(code);
}

Status::Status() : code_(ErrorCode::success), rep_(nullptr) {}

Status::Status(ErrorCode code, std::string_view msg)
    : code_(code), rep_(nullptr) {
  if (!msg.empty()) rep_ = std::make_shared<std::string>(msg);
  if (!Ok()) {
    LOGD("{}: {}", code, msg);
  }
}

Status::Status(bool b) : Status() {
  code_ = b ? ErrorCode::success : ErrorCode::internal_error;
}

Status::Status(Status&& rhs) noexcept : Status(false) {
  std::swap(code_, rhs.code_);
  std::swap(rep_, rhs.rep_);
}

Status& Status::operator=(Status&& rhs) noexcept {
  if (this != &rhs) {
    code_ = rhs.code_;
    rep_ = std::move(rhs.rep_);
    rhs.code_ = ErrorCode::success;
    rhs.rep_ = nullptr;
  }
  return *this;
}

Status::~Status() = default;

bool Status::Ok() const { return RawCode() >= 0; }

bool Status::Fatal() const { return code_ == ErrorCode::success; }

ErrorCode Status::Code() const { return code_; }

int Status::RawCode() const { return static_cast<int>(code_); }

std::string Status::ToString() const { return StatusCodeToString(code_); }

Status ToWarning(const Status& sts) {
  if (sts.Ok()) return sts;
  LOGT("Convert {} to warning", StatusCodeToString(sts.code_));
  return {ErrorCode::warning, *sts.rep_};
}

VilaFatalException::VilaFatalException(
    const CodeLocation& location,
    std::string_view msg,
    const std::vector<std::string>& bt
) {
  std::string ss = location.ToString();
  ss += fmt::format(" {}\n", msg);
  if (!bt.empty()) {
    ss += "Stacktrace:\n";
    std::for_each(++bt.cbegin(), bt.cend(), [&ss](const std::string& it) {
      ss += it;
      ss.push_back('\n');
    });
  }
  what_ = ss;
}
}  // namespace vila
