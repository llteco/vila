---
name: vila-status
description: Guidance for using the vila/status component - Status and StatusOr error handling pattern similar to absl::Status. Use this skill when implementing error handling in C++, using StatusOr<T> for return values, creating error Status objects, or handling ErrorCode enum values.
---

# VILA Status - Error Handling

Status and StatusOr pattern for error handling, similar to absl::Status/absl::StatusOr.

## Headers

```cpp
#include "vila/status/status.h"      // Status class and ErrorCode
#include "vila/status/statusor.h"     // StatusOr<T> template
```

## Bazel Target

```bazel
deps = ["//vila/status"]
```

## ErrorCode Enum

```cpp
#include "vila/status/status.h"

namespace vila {
enum class ErrorCode : int {
    warning = 1,                    // Not an error
    success = 0,                    // No error
    unknown = -1,
    invalid_configuration = -2,
    invalid_arguments = -3,
    object_not_found = -4,
    type_error = -5,
    unsupported = -6,
    bad_alloc = -7,
    calculator_init_failed = -8,
    calculator_aborted = -9,
    wait_time_out = -10,
    invalid_call = -11,
    calculator_skip = -12,
    null_pointer = -13,
    not_implemented = -14,
    // Hardware-specific errors
    d3d_internal_error = -15,
    ocl_internal_error = -16,
    ze_internal_error = -17,
    vpe_internal_error = -18,
    // Memory errors
    invalid_memory_shape = -19,
    invalid_memory_layout = -20,
    invalid_memory_format = -21,
    context_changed = -30,
    internal_error = -99,
};
}

// Convert to string
std::string str = vila::StatusCodeToString(code);

// Format with fmt
fmt::format("Error: {}", code);
```

## Status Class

```cpp
#include "vila/status/status.h"

// Success status
vila::Status ok = vila::Success();
vila::Status ok;  // default constructor = success

ok.Ok();     // true for success
ok.Fatal();  // true for error (not warning)

// Error status
vila::Status err(vila::ErrorCode::invalid_arguments, "argument must be positive");
err.Code();          // ErrorCode::invalid_arguments
err.RawCode();       // -3
err.ToString();      // "invalid_arguments: argument must be positive"

// Comparison
if (status == vila::ErrorCode::success) { ... }

// Convert error to warning
vila::Status warning = vila::ToWarning(err);
warning.Ok();     // false
warning.Fatal();  // false (it's a warning, not error)
```

## Error Constructor Functions

```cpp
// Each returns Status with formatted message
vila::Status Unknown("unknown error: {}", detail);
vila::Status InvalidConfiguration("config missing key: {}", key);
vila::Status InvalidArguments("arg {} out of range", arg);
vila::Status ObjectNotFound("object '{}' not found", name);
vila::Status TypeError("expected int, got {}", type);
vila::Status Unsupported("operation '{}' not supported", op);
vila::Status BadAlloc("failed to allocate {} bytes", size);
vila::Status NullPointer("pointer is null at {}", location);
vila::Status NotImplemented("feature '{}' not implemented", feature);
vila::Status InternalError("internal error: {}", msg);

// Hardware-specific
vila::Status D3dInternalError("D3D error: {}", msg);
vila::Status OclInternalError("OpenCL error: {}", msg);
vila::Status ZeInternalError("Level-Zero error: {}", msg);

// Memory-specific
vila::Status InvalidShape("shape {} invalid", shape);
vila::Status InvalidLayout("layout mismatch");
vila::Status InvalidFormat("format {} unsupported", fmt);

// Other
vila::Status CalculatorInitFailed("init failed for {}", name);
vila::Status CalculatorAborted("calculator {} aborted", name);
vila::Status CalculatorSkip("skipping {}", name);
vila::Status WaitTimeout("timeout after {} ms", ms);
vila::Status ContextChanged("context changed");
```

## StatusOr<T>

```cpp
#include "vila/status/statusor.h"

// StatusOr<T> is either a value or an error
vila::StatusOr<int> result = ComputeValue();

// Check success
if (result.Ok()) {
    int value = result.ValueOrDie();
    int value = std::move(result).ValueOrDie();  // move value
} else {
    vila::Status status = result.status();
    HandleError(status);
}

// Ignore error (suppress unused warnings)
result.IgnoreError();

// Return from function
vila::StatusOr<std::unique_ptr<Object>> CreateObject(int arg) {
    if (arg < 0) {
        return vila::InvalidArguments("arg must be non-negative");
    }
    return std::make_unique<Object>(arg);  // implicit conversion
}

// Consume value (moves out)
int value = result.ConsumeValueOrDie();

// Conversion between StatusOr types
vila::StatusOr<double> d = ComputeDouble();
vila::StatusOr<float> f = d;  // implicit conversion if T convertible
```

## VilaFatalException

```cpp
#include "vila/status/status.h"

// Exception for fatal errors (throw sparingly)
throw vila::VilaFatalException(
    vila::CodeLocation(__FILE__, __LINE__, __FUNCTION__),
    "fatal error message",
    vila::CaptureBackTrace()
);
```

## Common Patterns

### Function Return Pattern

```cpp
vila::StatusOr<Data*> LoadData(const std::string& path) {
    if (path.empty()) {
        return vila::InvalidArguments("path cannot be empty");
    }

    Data* data = TryLoad(path);
    if (!data) {
        return vila::ObjectNotFound("data not found at {}", path);
    }

    return data;  // success
}

// Caller
vila::StatusOr<Data*> result = LoadData("data.bin");
if (!result.Ok()) {
    LOGE("Failed: {}", result.status());
    return result.status();
}
Data* data = result.ValueOrDie();
```

### Chained Operations

```cpp
vila::Status ProcessPipeline() {
    vila::StatusOr<Input> input = ReadInput();
    if (!input.Ok()) return input.status();

    vila::StatusOr<Transformed> transformed = Transform(input.ValueOrDie());
    if (!transformed.Ok()) return transformed.status();

    vila::StatusOr<Output> output = WriteOutput(transformed.ValueOrDie());
    if (!output.Ok()) return output.status();

    return vila::Success();
}
```

### Factory Pattern

```cpp
class WidgetFactory {
public:
    static vila::StatusOr<std::unique_ptr<Widget>> Create(const Config& cfg) {
        if (!cfg.Validate()) {
            return vila::InvalidConfiguration("invalid config");
        }
        auto widget = std::make_unique<Widget>();
        vila::Status init_status = widget->Initialize(cfg);
        if (!init_status.Ok()) {
            return init_status;
        }
        return widget;
    }
};
```
