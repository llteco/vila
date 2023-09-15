/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2023 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 *
 * Description:
 * ~~~~~~~~~~~~
 * === Defining a registry ================================================
 *
 *  class Widget {};
 *
 *  using WidgetRegistry =
 *      GlobalFactoryRegistry<unique_ptr<Widget>,                 // return
 *                            unique_ptr<Gadget>, const Thing*>   // args
 *
 * === Registering an implementation =======================================
 *
 *  class MyWidget : public Widget {
 *    static unique_ptr<Widget> Create(unique_ptr<Gadget> arg,
 *                                     const Thing* thing) {
 *      return MakeUnique<Widget>(std::move(arg), thing);
 *    }
 *    ...
 *  };
 *
 *  REGISTER_FACTORY_FUNCTION_QUALIFIED(
 *      WidgetRegistry, widget_registration,
 *      ::my_ns::MyWidget, MyWidget::Create);
 *
 * === Using std::function =================================================
 *
 *  class Client {};
 *
 *  using ClientRegistry =
 *      GlobalFactoryRegistry<vila::StatusOr<unique_ptr<Client>>;
 *
 *  class MyClient : public Client {
 *   public:
 *    MyClient(unique_ptr<Backend> backend)
 *      : backend_(std::move(backend)) {}
 *   private:
 *     const std::unique_ptr<Backend> backend_;
 *  };
 *
 *  // Any std::function that returns a Client is valid to pass here. Below,
 *  // we use a lambda.
 *  REGISTER_FACTORY_FUNCTION_QUALIFIED(
 *      ClientRegistry, client_registration,
 *      ::my_ns::MyClient,
 *      []() {
 *        auto backend = absl::make_unique<Backend>("/path/to/backend");
 *        const vila::Status status = backend->Init();
 *        if (!status.Ok()) {
 *          return status;
 *        }
 *        std::unique_ptr<Client> client
 *            = absl::make_unique<MyClient>(std::move(backend));
 *        return client;
 *      });
 *
 * === Using the registry to create instances ==============================
 *
 *  // Registry will return vila::StatusOr<Object>
 *  vila::StatusOr<unique_ptr<Widget>> s_or_widget =
 *      WidgetRegistry::CreateByName(
 *          "my_ns.MyWidget", std::move(gadget), thing);
 *  // Registry will return NOT_FOUND if the name is unknown.
 *  if (!s_or_widget.Ok()) ... // handle error
 *  DoStuffWithWidget(std::move(s_or_widget).ValueOrDie());
 *
 *  // It's also possible to find an instance by name within a source namespace.
 *  auto s_or_widget = WidgetRegistry::CreateByNameInNamespace(
 *      "my_ns.sub_namespace", "MyWidget");
 *
 *  // It's also possible to just check if a name is registered without creating
 *  // an instance.
 *  bool registered = WidgetRegistry::IsRegistered("my_ns::MyWidget");
 *
 *  // It's also possible to iterate through all registered function names.
 *  // This might be useful if clients outside of your codebase are registering
 *  // plugins.
 *  for (const auto& name : WidgetRegistry::GetRegisteredNames()) {
 *    vila::StatusOr<unique_ptr<Widget>> s_or_widget =
 *        WidgetRegistry::CreateByName(name, std::move(gadget), thing);
 *    ...
 *  }
 *
 * === Injecting instances for testing =====================================
 *
 * unregister unregisterer(WidgetRegistry::Register(
 *     "MockWidget",
 *      [](unique_ptr<Gadget> arg, const Thing* thing) {
 *        ...
 *      }));
 ******************************************************************************/
#ifndef VILA_WIDGET_REGISTRATION_H_
#define VILA_WIDGET_REGISTRATION_H_

#include <algorithm>
#include <functional>
#include <mutex>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "vila/widget/registration_token.h"

namespace vila {
template <typename R, typename... Args>
class FunctionRegistry {
 public:
  using Function = std::function<R(Args...)>;
  using ReturnType = R;

  FunctionRegistry() = default;

  FunctionRegistry(const FunctionRegistry&) = delete;
  FunctionRegistry& operator=(const FunctionRegistry&) = delete;

  RegistrationToken Register(
      const std::string& name, Function func, const std::string& filename
  ) {
    std::string normalized_name = GetNormalizedName(name);
    std::unique_lock<std::mutex> lock(lock_);
    if (functions_.insert(std::make_pair(normalized_name, std::move(func)))
            .second) {
      locations_.insert(std::make_pair(normalized_name, filename));
      return RegistrationToken([this, normalized_name]() {
        unregister(normalized_name);
      });
    }
    // LOG(FATAL) << "Function with name " << name << " already registered.";
    return RegistrationToken([]() {});
  }

  // Force 'args' to be deduced by templating the function, instead of just
  // accepting Args. This is necessary to make 'args' a forwarding reference as
  // opposed to a plain rvalue reference.
  // https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers
  //
  // The absl::enable_if_t is used to disable this method if Args2 are not
  // convertible to Args. This will allow the compiler to identify the offending
  // line (i.e. the line where the method is called) in the first error message,
  // rather than nesting it multiple levels down the error stack.
  template <
      typename... Args2,
      std::enable_if_t<
          std::is_convertible<std::tuple<Args2...>, std::tuple<Args...>>::value,
          int> = 0>
  ReturnType Invoke(const std::string& name, Args2&&... args) {
    std::string normalized_name = GetNormalizedName(name);
    Function function;
    {
      std::unique_lock<std::mutex> lock(lock_);
      auto it = functions_.find(normalized_name);
      if (it == functions_.end()) {
        return {};
      }
      function = it->second;
    }
    return function(std::forward<Args2>(args)...);
  }

  // Note that it's possible for registered implementations to be subsequently
  // unregistered, though this will never happen with registrations made via
  // REGISTER_FACTORY_FUNCTION.
  bool IsRegistered(const std::string& name) const {
    std::string normalized_name = GetNormalizedName(name);
    std::unique_lock<std::mutex> lock(lock_);
    return functions_.count(normalized_name) != 0;
  }

  // Returns a vector of all registered function names.
  // Note that it's possible for registered implementations to be subsequently
  // unregistered, though this will never happen with registrations made via
  // REGISTER_FACTORY_FUNCTION.
  std::unordered_set<std::string> GetRegisteredNames() const {
    std::unique_lock<std::mutex> lock(lock_);
    std::unordered_set<std::string> names;
    std::for_each(
        functions_.cbegin(), functions_.cend(),
        [&names](const std::pair<const std::string, Function>& pair) {
          names.insert(pair.first);
        }
    );
    return names;
  }

  // Normalizes a C++ qualified name.  Validates the name qualification.
  // The name must be either unqualified or fully qualified with a leading "::".
  // The leading "::" in a fully qualified name is stripped.
  std::string GetNormalizedName(const std::string& name) const {
    auto cxx_split_pos = name.find("::");
    if (cxx_split_pos == 0) {
      return {name.begin() + 2, name.end()};
    }
    return name;
  }

  std::string GetLocation(const std::string& name) const {
    std::string normalized_name = GetNormalizedName(name);
    std::unique_lock<std::mutex> lock(lock_);
    auto it = locations_.find(normalized_name);
    if (it == locations_.end()) {
      return "";
    }
    return locations_.at(normalized_name);
  }

 private:
  //!< all actions on `m_functions` should add a lock.
  mutable std::mutex lock_;
  std::unordered_map<std::string, Function> functions_;
  std::unordered_map<std::string, std::string> locations_;

  void unregister(const std::string& name) {
    std::unique_lock<std::mutex> lock(lock_);
    functions_.erase(name);
    locations_.erase(name);
  }
};

template <typename R, typename... Args>
class GlobalFactoryRegistry {
  using Functions = FunctionRegistry<R, Args...>;

 public:
  static RegistrationToken Register(
      const std::string& name,
      typename Functions::Function func,
      const std::string& filename = ""
  ) {
    return functions()->Register(name, std::move(func), filename);
  }

  // Invokes the specified factory function and returns the result.
  // If using namespaces with this registry, the variant with a namespace
  // argument should be used.
  template <typename... Args2>
  static typename Functions::ReturnType CreateByName(
      const std::string& name, Args2&&... args
  ) {
    return functions()->Invoke(name, std::forward<Args2>(args)...);
  }

  // Returns file basename of registered class.
  static std::string GetLocationByName(const std::string& name) {
    return functions()->GetLocation(name);
  }

  // Returns true if the specified factory function is available.
  // If using namespaces with this registry, the variant with a namespace
  // argument should be used.
  static bool IsRegistered(const std::string& name) {
    return functions()->IsRegistered(name);
  }

  static std::unordered_set<std::string> GetRegisteredNames() {
    return functions()->GetRegisteredNames();
  }

  GlobalFactoryRegistry() = delete;

 private:
  // Returns the factory function registry singleton.
  static Functions* functions() {
    static Functions functions = Functions();
    return &functions;
  }
};
}  // namespace vila

// Two levels of macros are required to convert __LINE__ into a std::string
// containing the line number.
#define REGISTRY_STATIC_VAR_INNER(var_name, line) var_name##_##line##__
#define REGISTRY_STATIC_VAR(var_name, line) \
  REGISTRY_STATIC_VAR_INNER(var_name, line)

#define REGISTER_FACTORY_FUNCTION(RegistryType, name, ...)         \
  static auto REGISTRY_STATIC_VAR(registration_##name, __LINE__) = \
      vila::RegistrationToken(RegistryType::Register(#name, __VA_ARGS__))

#define REGISTER_FACTORY_FUNCTION_QUALIFIED(RegistryType, var_name, name, ...) \
  static auto REGISTRY_STATIC_VAR(var_name, __LINE__) =                        \
      vila::RegistrationToken(RegistryType::Register(#name, __VA_ARGS__))

#endif  // VILA_WIDGET_REGISTRATION_H_
