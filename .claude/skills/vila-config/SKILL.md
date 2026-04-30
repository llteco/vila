---
name: vila-config
description: Guidance for using the vila/config component - JSON configuration system using nlohmann/json. Use this skill when working with JSON configuration in C++, parsing JSON files, reading configuration from streams, or using vila's Json type alias and configuration utilities.
---

# VILA Config - JSON Configuration

JSON-based configuration system built on nlohmann/json.

## Headers

```cpp
#include "vila/config/configuration.h"
```

## Bazel Target

```bazel
deps = ["//vila/config"]
```

## Usage

Vila provides a type alias and convenience functions for nlohmann::json:

```cpp
#include "vila/config/configuration.h"

namespace vila {
using Json = nlohmann::json;
}

// Read JSON from file
vila::Json config = vila::JsonFromFile("config.json");

// Read JSON from stream
std::ifstream file("config.json");
vila::Json config = vila::JsonFromStream(file);

// Read JSON from string document
std::string json_str = "{\"key\": \"value\"}";
vila::Json config = vila::JsonFromDoc(json_str);
```

## Nlohmann/Json Operations

The `vila::Json` type alias provides full nlohmann::json functionality:

```cpp
vila::Json j;
j["name"] = "example";
j["value"] = 42;
j["array"] = {1, 2, 3};
j["nested"]["key"] = "nested_value";

// Access values
std::string name = j["name"];
int value = j["value"];

// Check if key exists
if (j.contains("optional_key")) { ... }

// Get value with default
int val = j.value("key", 0);  // returns 0 if key missing

// Parse string
vila::Json j = vila::Json::parse("{\"key\": 123}");

// Serialize
std::string str = j.dump();       // compact
std::string str = j.dump(2);      // pretty-print with 2-space indent

// Type checking
j.is_string();
j.is_number();
j.is_array();
j.is_object();
j.is_null();

// Array operations
for (auto& element : j["array"]) { ... }
j["array"].push_back(4);
j["array"].size();

// Object operations
for (auto& [key, value] : j.items()) { ... }
j.erase("key");
```

## Configuration Pattern

Common pattern for application configuration:

```cpp
struct AppConfig {
    std::string name;
    int port;
    std::vector<std::string> hosts;

    static AppConfig FromJson(const vila::Json& j) {
        AppConfig cfg;
        cfg.name = j.value("name", "default");
        cfg.port = j.value("port", 8080);
        cfg.hosts = j.value("hosts", std::vector<std::string>{});
        return cfg;
    }
};

vila::Json config_json = vila::JsonFromFile("app.json");
AppConfig config = AppConfig::FromJson(config_json);
```
