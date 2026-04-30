---
name: vila-widget
description: Guidance for using the vila/widget component - registration system for factory pattern and plugin architecture. Use this skill when implementing factory registries, registering plugin implementations, using GlobalFactoryRegistry, or managing RegistrationToken for lifecycle control.
---

# VILA Widget - Registration System

Factory registry pattern for plugin architectures and runtime registration.

## Headers

```cpp
#include "vila/widget/registration.h"       // FunctionRegistry, GlobalFactoryRegistry
#include "vila/widget/registration_token.h"  // RegistrationToken, RAII Unregister
```

## Bazel Target

```bazel
deps = ["//vila/widget:registration"]
```

## GlobalFactoryRegistry

Singleton registry for factory functions:

```cpp
#include "vila/widget/registration.h"

// Define registry type
using WidgetRegistry = vila::GlobalFactoryRegistry<
    std::unique_ptr<Widget>,        // return type
    std::unique_ptr<Gadget>,        // arg1
    const Config*                   // arg2
>;

// Register factory function
static auto registration = vila::RegistrationToken(
    WidgetRegistry::Register("MyWidget",
        [](std::unique_ptr<Gadget> g, const Config* c) {
            return std::make_unique<MyWidget>(std::move(g), c);
        },
        __FILE__  // optional: source location
    )
);

// Create by name
vila::StatusOr<std::unique_ptr<Widget>> widget =
    WidgetRegistry::CreateByName("MyWidget", gadget, config);

// Check if registered
bool exists = WidgetRegistry::IsRegistered("MyWidget");

// Get all registered names
auto names = WidgetRegistry::GetRegisteredNames();

// Get source location
std::string loc = WidgetRegistry::GetLocationByName("MyWidget");
```

## Registration Macros

```cpp
#include "vila/widget/registration.h"

// Register with unqualified name
REGISTER_FACTORY_FUNCTION(WidgetRegistry, MyWidget,
    [](Args... args) { return std::make_unique<MyWidget>(args...); });

// Register with qualified name and custom variable
REGISTER_FACTORY_FUNCTION_QUALIFIED(WidgetRegistry, my_widget_reg, ::my_ns::MyWidget,
    [](Args... args) { return std::make_unique<MyWidget>(args...); });
```

## RegistrationToken

Token for managing registration lifetime:

```cpp
#include "vila/widget/registration_token.h"

// Create token with unregister function
vila::RegistrationToken token([]() { Cleanup(); });

// Unregister manually
token.Unregister();  // safe to call multiple times

// Combine multiple tokens
auto combined = vila::RegistrationToken::Combine({token1, token2});
combined.Unregister();  // unregisters all
```

## RAII Unregister

Automatic cleanup on scope exit:

```cpp
#include "vila/widget/registration_token.h"

// RAII wrapper
vila::Unregister guard(WidgetRegistry::Register("TempWidget", factory));
// ... use widget ...
// guard destructor calls Unregister()

// Reset with new token (unregisters old first)
guard.Reset(new_token);

// Move ownership
vila::Unregister other = std::move(guard);
```

## FunctionRegistry

Non-singleton registry for local use:

```cpp
#include "vila/widget/registration.h"

vila::FunctionRegistry<int, std::string> registry;

// Register function
auto token = registry.Register("calc", [](std::string s) { return s.size(); });

// Invoke
int result = registry.Invoke("calc", "hello");

// Check and list
bool exists = registry.IsRegistered("calc");
auto names = registry.GetRegisteredNames();

// Get location
std::string loc = registry.GetLocation("calc");
```

## Plugin Pattern

```cpp
// Define interface
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void Execute() = 0;
};

// Registry for plugins
using PluginRegistry = vila::GlobalFactoryRegistry<std::unique_ptr<Plugin>>;

// Plugin implementation
class MyPlugin : public Plugin {
public:
    static std::unique_ptr<Plugin> Create() {
        return std::make_unique<MyPlugin>();
    }

    void Execute() override { /* ... */ }
};

// Register
REGISTER_FACTORY_FUNCTION_QUALIFIED(PluginRegistry, my_plugin, ::MyPlugin, MyPlugin::Create);

// Load and use
void LoadPlugin(const std::string& name) {
    auto plugin = PluginRegistry::CreateByName(name);
    if (plugin.Ok()) {
        plugin.ValueOrDie()->Execute();
    }
}

// List available plugins
for (const auto& name : PluginRegistry::GetRegisteredNames()) {
    std::cout << "Available: " << name << "\n";
}
```

## Testing Pattern

```cpp
// Inject mock for testing
class MockWidget : public Widget { /* ... */ };

vila::Unregister mock_reg(WidgetRegistry::Register(
    "MockWidget",
    [](auto args) { return std::make_unique<MockWidget>(); }
));

// Test with mock
auto widget = WidgetRegistry::CreateByName("MockWidget", args);
// mock_reg unregisters automatically at scope end
```

## Notes

- `GlobalFactoryRegistry` is a singleton - one registry per `R, Args...` signature
- Registration names should be qualified C++ names (e.g., `::my_ns::MyClass`)
- Macro registrations are static - persist until program exit
- Manual `Register()` returns token for temporary/test registrations
- Thread-safe: internal mutex protects all operations
