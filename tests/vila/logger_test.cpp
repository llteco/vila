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
 * Description: test logger
 ****************************************/
#include "vila/logging/logger.h"

#include <gtest/gtest.h>

#include "vila/profiling/trace.h"

TEST(Logger, ConsoleWrite) {
  LOGT("This is a trace log");
  LOGD("This is a debug log");
  LOGI("This is an info log");
  LOGW("This is a warning log");
  LOGE("This is an error log");
}

TEST(Tracer, TraceDump) {
  LOGI("binary tracer test");
  DEBUGTRACE();                               // log entry in debug mode
  vila::BinTracer::Instance().StepFrameId();  // frame -> 1
  std::vector<char> bin{'a', 'b', 'c', 'd', 'e'};
  vila::BinTracer::Instance().Trace("foo", bin.data(), {bin.size()});
  vila::BinTracer::Instance().StepFrameId();  // frame -> 2
  bin.resize(100 * 100 * 100);
  vila::BinTracer::Instance().Trace("big", bin.data(), {bin.size()});
}
