---
name: vila-logging
description: Guidance for using the vila/logging component - unified logging system with WPP (Windows Trace Preprocessing) support and fmt formatting. Use this skill when implementing logging in C++, using LOGT/LOGD/LOGI/LOGW/LOGE macros, or capturing source code locations and backtraces.
---

# VILA Logging - Unified Log System

Singleton logging system with trace/debug/info/warning/error levels, fmt formatting, and optional WPP support.

## Headers

```cpp
#include "vila/logging/logger.h"
#include "vila/logging/code_location.h"  // Source location capture
```

## Bazel Target

```bazel
deps = ["//vila/logging:logger"]
# For code_location:
deps = ["//vila/logging:code_location"]
```

## Build Flags

- `--enable_wpp`: Enable Windows WPP logging (default: false)
- `define VILA_USE_LOG`: Enable logging macros (auto-defined when `--with_log=true`)

## Logger Class

```cpp
#include "vila/logging/logger.h"

namespace vila {
class Logger {
    enum {
        LOG_LEVEL_TRACE = 0,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
    };
};
}

// Get singleton
vila::Logger* logger = vila::Logger::Get();

// Set log level (positive=console, negative=file)
vila::Logger::SetLoggerLevel(2);   // LOG_LEVEL_INFO and above to console
vila::Logger::SetLoggerLevel(-2);  // LOG_LEVEL_INFO and above to file

// Check if should log
if (vila::Logger::ShouldLog(vila::Logger::LOG_LEVEL_DEBUG)) { ... }

// Nested logger (with scoped name)
vila::Logger* nested = logger->Nest("module_name");

// Log methods (use fmt formatting)
logger->Trace("value = {}", value);
logger->Debug("processing item {}", id);
logger->Info("operation completed");
logger->Warning("unexpected value: {}", val);
logger->Error("failed to open file: {}", filename);

// Flush buffered logs
vila::Logger::Flush();

// Drop logger (cleanup)
vila::Logger::Drop();
```

## Logging Macros

When `VILA_USE_LOG` is defined:

```cpp
// Global logger macros
LOGT("trace message {}", arg);  // Trace
LOGD("debug message {}", arg);  // Debug
LOGI("info message {}", arg);   // Info
LOGW("warning {}", arg);        // Warning
LOGE("error {}", arg);          // Error

// Nested logger macros (scoped with file stem)
VLOGT("trace {}", arg);   // Uses FILESTEM(__FILE__) as nested name
VLOGD("debug {}", arg);
VLOGI("info {}", arg);
VLOGW("warning {}", arg);
VLOGE("error {}", arg);
```

## Code Location

Captures source location for error reporting:

```cpp
#include "vila/logging/code_location.h"

vila::CodeLocation loc(__FILE__, __LINE__, __FUNCTION__);
std::string str = loc.ToString();  // "file.cpp (42) void func()"

// Capture backtrace
std::vector<std::string> bt = vila::CaptureBackTrace();

// Format with fmt
fmt::format("Error at {}", loc);
```

## Log Level Guidance

| Level   | Use Case                              |
|---------|---------------------------------------|
| Trace   | Detailed execution flow, enter/exit   |
| Debug   | Development info, variable values     |
| Info    | Normal operation milestones           |
| Warning | Unexpected but recoverable situations |
| Error   | Failures, exceptions                  |

## Pattern: Scoped Logging

```cpp
void ProcessData(Data* data) {
    VLOGI("ProcessData start, size={}", data->size());
    // ... processing ...
    VLOGD("intermediate result={}", result);
    // ... more processing ...
    VLOGI("ProcessData complete");
}
```

## Pattern: Error with Location

```cpp
#include "vila/logging/code_location.h"
#include "vila/logging/logger.h"

void HandleError(const std::string& msg) {
    vila::CodeLocation loc(__FILE__, __LINE__, __FUNCTION__);
    auto bt = vila::CaptureBackTrace();
    LOGE("Error at {}: {}", loc.ToString(), msg);
    for (const auto& frame : bt) {
        LOGT("  {}", frame);
    }
}
```
