---
name: vila-profiling
description: Guidance for using the vila/profiling component - profiling tools including timer, ITT instrumentation, and tracing utilities. Use this skill when profiling C++ code performance, measuring execution time, using Intel ITT API, or adding trace markers for performance analysis.
---

# VILA Profiling - Performance Tools

Profiling utilities for measuring execution time, ITT instrumentation, and tracing.

## Headers

```cpp
#include "vila/profiling/timer.h"   // CPU profiler
#include "vila/profiling/itt.h"     // Intel ITT instrumentation
#include "vila/profiling/trace.h"   // Binary tracing
```

## Bazel Targets

```bazel
deps = [
    "//vila/profiling:timer",
    "//vila/profiling:itt",
    "//vila/profiling:tracer",
]
```

## Profiler (timer.h)

Singleton profiler for recording CPU execution time:

```cpp
#include "vila/profiling/timer.h"

namespace vila {
enum class ProfileFormat { text, json };
}

// Get singleton
vila::Profiler* profiler = vila::Profiler::Instance();

// Manual profiling
profiler->StartProfile("operation_name");
// ... do work ...
profiler->EndProfile("operation_name");

// Get recorded times
std::vector<uint64_t> times = profiler->GetProfile("operation_name");
// times are in nanoseconds

// Add custom timing
profiler->AddProfile("custom", std::chrono::nanoseconds(123456));

// Print analysis
profiler->PrintAnalyze();                   // text format
profiler->PrintAnalyze(vila::ProfileFormat::json);  // JSON format
```

## AutoProfiler (RAII)

```cpp
#include "vila/profiling/timer.h"

// Automatic scope profiling
void MyFunction() {
    vila::AutoProfiler prof("MyFunction");
    // ... work ...
    // Profiler.EndProfile called on destruction
}
```

## Windows Timer Period

More accurate timing on Windows:

```cpp
#ifdef _WIN32
#include "vila/profiling/timer.h"

// RAII timer period adjustment
vila::AutoSetTimerPeriod timer_period;
// Higher accuracy timing active for scope

// Manual control
vila::SetTimerPeriod::Instance()->Start();
vila::SetTimerPeriod::Instance()->End();
#endif
```

## ITT Instrumentation (itt.h)

Intel ITT API for VTune/Intel Profiler integration:

```cpp
#include "vila/profiling/itt.h"

// Create domain
vila::itt::domain_t* domain = vila::itt::DomainManager::CreateDomain("my_domain");

// Frame scope marking
void ProcessFrame() {
    vila::itt::FrameScope frame(domain);  // marks frame boundary
    // ... frame processing ...
    // Frame end on destruction
}

// Or use string name
vila::itt::FrameScope frame("my_domain");
```

## Binary Tracer (trace.h)

Trace binary data for debugging:

```cpp
#include "vila/profiling/trace.h"

// Environment variables:
// VILA_TRACE_DEBUG=N     - start frame N
// VILA_TRACE_DEBUG=N,M   - frames N through M

vila::BinTracer& tracer = vila::BinTracer::Instance();

// Step frame counter
tracer.StepFrameId();

// Check if should trace current frame
if (tracer.ShouldTrace()) {
    // Trace binary data with shape
    tracer.Trace("tensor_output", data_ptr, height, width, channels);
}

// Type-specific overloads
tracer.Trace("char_data", char_ptr, shape);
tracer.Trace("int_data", int_ptr, shape);
tracer.Trace("float_data", float_ptr, shape);
tracer.Trace("double_data", double_ptr, shape);
```

## Debug Trace Macro

```cpp
#include "vila/profiling/trace.h"

#ifdef _DEBUG
// Logs function entry and exit at trace level
void MyFunction() {
    DEBUGTRACE();  // Logs "MyFunction +" on entry, "MyFunction -" on exit
    // ... work ...
}
#else
#define DEBUGTRACE(...)  // No-op in release
#endif
```

## Common Patterns

### Benchmark a Function

```cpp
void BenchmarkOperation() {
    vila::Profiler::Instance()->StartProfile("benchmark");
    for (int i = 0; i < 1000; i++) {
        vila::AutoProfiler prof("inner_op");
        OperationToMeasure();
    }
    vila::Profiler::Instance()->EndProfile("benchmark");
    vila::Profiler::Instance()->PrintAnalyze();
}
```

### Mark Pipeline Frames

```cpp
void ProcessPipeline() {
    static auto* domain = vila::itt::DomainManager::CreateDomain("pipeline");

    while (running) {
        vila::itt::FrameScope frame(domain);
        ProcessFrame();
        vila::BinTracer::Instance()->StepFrameId();
    }
}
```
