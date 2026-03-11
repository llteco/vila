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
 * Description: tracing binary objects
 ****************************************/
#include "vila/profiling/trace.h"

#include <fmt/format.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <fstream>
#include <limits>
#include <memory>
#include <mutex>
#include <string>

#include "clim/os.h"
#include "clim/os_path.h"
#include "clim/str_split.h"
#include "clim/vt/vt.h"

namespace vila {
#ifndef MAKEFOURCC
constexpr uint32_t MAKEFOURCC(int ch1, int ch2, int ch3, int ch4) {
  return (ch4 << (8 * 3)) | (ch3 << (8 * 2)) | (ch2 << 8) | ch1;
}
#endif

class BinarySink : public spdlog::sinks::base_sink<std::mutex> {
 public:
  explicit BinarySink(const std::string& filename) {
    file_.open(filename, std::ios::binary | std::ios::app);
  }

  ~BinarySink() override = default;

 private:
  void sink_it_(const spdlog::details::log_msg& msg) override {
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
    file_.write(formatted.data(), formatted.size());
  }

  void flush_() override { file_.flush(); }

  std::ofstream file_;
};

constexpr uint32_t kBinTag = MAKEFOURCC('B', 'I', 'N', 'T');

struct BinHead {
  const uint32_t magic = kBinTag;
  uint64_t length = sizeof(BinHead);
  uint32_t dtype = 0;
  uint64_t frame = 0;
  uint32_t tag_length = 0;
  uint32_t dims = 1;
};

template <class T>
struct DtypeTrait {
  // default type id, unknown
  enum : uint32_t { ID = 0 };
};

template <>
struct DtypeTrait<char> {
  // uint8 type id
  enum : uint32_t { ID = 1 };
};

template <>
struct DtypeTrait<int> {
  // uint32 type, or RGBA packed color
  enum : uint32_t { ID = 2 };
};

template <>
struct DtypeTrait<float> {
  // float type id
  enum : uint32_t { ID = 3 };
};

template <>
struct DtypeTrait<double> {
  // double type id
  enum : uint32_t { ID = 4 };
};

template <>
struct DtypeTrait<BinTracer::char3> {
  // char3 type id for RGB packed color
  enum : uint32_t { ID = 5 };
};

class BinTracerImpl : public BinTracer {
 public:
  BinTracerImpl() {
    // get vila logger
    logger_ = std::make_shared<spdlog::logger>("bintrace");
    // only trace on the lowest log level
    if (logger_ && Environ().count("VILA_TRACE_DEBUG")) {
      auto path = std::filesystem::current_path() / "vila.bintrace";
      auto sink = std::make_shared<BinarySink>(path.string());
      sink->set_pattern("%v");
      logger_->sinks().push_back(sink);
      logger_->set_level(spdlog::level::level_enum::trace);
      auto info = StrSplit(Environ().at("VILA_TRACE_DEBUG"), ",");
      if (info.size() == 1) {
        start_ = std::strtoll(info[0].data(), nullptr, 10);
      } else if (info.size() == 2) {
        start_ = std::strtoll(info[0].data(), nullptr, 10);
        end_ = std::strtoll(info[1].data(), nullptr, 10);
      }
      LOGT("vila trace: {},{}", start_, end_);
    }
  }

  ~BinTracerImpl() override { logger_->flush(); }

  void StepFrameId() override { frame_id_++; }

  bool ShouldTrace() const override {
    return frame_id_ < end_ && frame_id_ >= start_;
  }

  void Trace(std::string_view tag, char* bin, const blob& shape) override {
    this->trace(tag, bin, shape);
  }

  void Trace(std::string_view tag, char3* bin, const blob& shape) override {
    this->trace(tag, bin, shape);
  }

  void Trace(std::string_view tag, int* bin, const blob& shape) override {
    this->trace(tag, bin, shape);
  }

  void Trace(std::string_view tag, float* bin, const blob& shape) override {
    this->trace(tag, bin, shape);
  }

  void Trace(std::string_view tag, double* bin, const blob& shape) override {
    this->trace(tag, bin, shape);
  }

 private:
  template <class T>
  void trace(std::string_view tag, T* bin, const blob& shape) const {
    BinHead head;
    size_t size = vt::PI(shape) * sizeof(T);
    head.length += tag.size() + size + shape.size() * sizeof(size_t);
    head.dtype = DtypeTrait<T>::ID;
    head.frame = frame_id_;
    head.tag_length = static_cast<uint32_t>(tag.size());
    head.dims = static_cast<uint32_t>(shape.size());
    fmt::bytes raw_shape(std::string_view(
        reinterpret_cast<const char*>(shape.data()),
        shape.size() * sizeof(size_t)
    ));
    fmt::bytes bytes(std::string_view(reinterpret_cast<char*>(bin), size));
    logger_->trace("{}{}{}{}", head, tag, raw_shape, bytes);
  }

  uint64_t frame_id_ = 0;
  uint64_t start_ = 0;
  uint64_t end_ = std::numeric_limits<uint64_t>::max();
  std::shared_ptr<spdlog::logger> logger_;
};

BinTracer& BinTracer::Instance() {
  static std::unique_ptr<BinTracerImpl> tracer;
  if (!tracer) {
    tracer = std::make_unique<BinTracerImpl>();
  }
  return *tracer;
}
}  // namespace vila

/**
 * @brief Formatter in {fmt} for type `PixFormat`
 *
 * This is a general template formatter for type `PixFormat`, inherited
 * from `basic_string_view<Char>`, so the format specifications such as {:}
 * follows the rules of basic string.
 * One method `format` must be implemented.
 * @note See https://fmt.dev/9.0.0/api.html#format-api
 * @example
 * fmt::format("{}", PixFormat::nv12);  // "nv12"
 */
template <typename Char>
struct fmt::formatter<vila::BinHead, Char>
    : fmt::formatter<fmt::basic_string_view<Char>> {
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-*) ignore function naming style
  auto format(const vila::BinHead& head, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    fmt::string_view bytes(reinterpret_cast<const char*>(&head), sizeof(head));
    return fmt::format_to(ctx.out(), "{}", fmt::bytes(bytes));
  }
};
