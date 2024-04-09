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
#ifndef CLIM_ARGPARSE_H_
#define CLIM_ARGPARSE_H_
#include <any>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "clim/str_split.h"

class ArgumentParser;

/**
 * @brief Represents errors during parsing.
 */
class ParseError : public std::exception {
 public:
  ParseError() = default;

  explicit ParseError(std::string_view msg) : msg_(msg) {}

  const char* what() const noexcept override { return msg_.c_str(); }  // NOLINT

 private:
  std::string msg_;
};

class Argument {
  enum class action { none, store_true, store_false };
  friend class ArgumentParser;

 public:
  Argument() = default;

  /**
   * @brief Construct a new Argument object with a name
   *
   * @param name: name of the argument can have 3 patterns:
   *   1. "foo": position argument
   *   2. "-f": short key
   *   3. "--foo": full key
   */
  explicit Argument(std::string_view name) : full_name_(name) {
    if (name.substr(0, 2) == "--") {
      name_ = name.substr(2);
    } else if (name.substr(0, 1) == "-") {
      name_ = name.substr(1);
    } else {
      name_ = name;
      required_ = true;
      nargs_ = 1;
    }
  }

  Argument& WithDefault(int64_t default_value) {
    def_ = default_value;
    return *this;
  }

  Argument& WithDefault(int32_t default_value) {
    def_ = static_cast<int64_t>(default_value);
    return *this;
  }

  Argument& WithDefault(float default_value) {
    def_ = default_value;
    return *this;
  }

  Argument& WithDefault(double default_value) {
    def_ = static_cast<float>(default_value);
    return *this;
  }

  Argument& WithDefault(std::string default_value) {
    def_ = std::move(default_value);
    return *this;
  }

  Argument& WithHelp(std::string_view message) {
    help_ = message;
    return *this;
  }

  Argument& Required() {
    required_ = true;
    return *this;
  }

  Argument& StoreTrue() {
    flag_ = action::store_true;
    def_ = false;
    return *this;
  }

  Argument& StoreFalse() {
    flag_ = action::store_false;
    def_ = true;
    return *this;
  }

  Argument& Nargs(uint64_t nargs) {
    if (static_cast<char>(nargs) == '*') {
      nargs_ = -1;
    } else if (static_cast<char>(nargs) == '+') {
      nargs_ = 0;
    } else {
      nargs_ = static_cast<int64_t>(nargs);
    }
    return *this;
  }

  bool IsFlag() const { return flag_ != action::none; }

  bool IsRequired() const { return required_; }

  bool None() const {
    switch (flag_) {
      case action::store_true:
      case action::store_false:
        return !std::any_cast<bool>(def_);
      default:
        return vals_.empty();
    }
  }

  float AsFloat() const {
    if (vals_.empty()) {
      return std::any_cast<float>(def_);
    }
    return std::stof(vals_[0]);
  }

  int64_t AsLong() const {
    if (vals_.empty()) {
      return std::any_cast<int64_t>(def_);
    }
    return std::stoll(vals_[0]);
  }

  std::string AsStr() const {
    switch (flag_) {
      case action::store_false:
      case action::store_true:
        return None() ? "false" : "true";
      default:
        break;
    }
    if (vals_.empty()) {
      return std::any_cast<std::string>(def_);
    }
    return vals_[0];
  }

  operator bool() const { return !None(); }  // NOLINT(*-explicit-*)

  operator float() const { return AsFloat(); }  // NOLINT(*-explicit-*)

  operator int64_t() const { return AsLong(); }  // NOLINT(*-explicit-*)

  operator std::string() const { return AsStr(); }  // NOLINT(*-explicit-*)

  Argument operator[](size_t pos) const {
    Argument arg = *this;
    arg.vals_.front() = arg.vals_[pos];
    return arg;
  }

  size_t Size() const { return vals_.size(); }

  bool operator==(std::string_view arg) const { return name_ == arg; }

  bool operator!=(std::string_view arg) const { return name_ != arg; }

 private:
  void setValue(std::string_view value) {
    switch (flag_) {
      case action::store_true:
        def_ = true;
        break;
      case action::store_false:
        def_ = false;
        break;
      default:
        if (!value.empty()) {
          vals_.emplace_back(value);
        }
        break;
    }
  }

  bool check(std::string& err_msg) const {
    std::stringstream ss;
    if (nargs_ > 0) {
      if (vals_.size() != static_cast<size_t>(nargs_)) {
        if ((vals_.empty() && required_) || !vals_.empty()) {
          ss << name_ << " expects " << nargs_ << " values but gets "
             << vals_.size();
          if (pos_ >= 0) {
            ss << "\n"
               << "can not specify position argument with --" << name_;
          }
        }
      }
    } else if (nargs_ == 0 && vals_.empty() && required_) {
      ss << name_ << " expects at least one value but gets none";
    }
    if (!ss.str().empty()) {
      err_msg += ss.str() + "\n";
      return false;
    }
    return true;
  }

  bool required_ = false;
  int64_t pos_ = -1;
  action flag_ = action::none;
  std::string name_;
  std::string full_name_;
  std::string help_;
  std::any def_ = std::string("");
  int64_t nargs_ = 1;
  std::vector<std::string> vals_;
};

/** @class ArgumentParser
 * @brief Object for parsing command line strings into c++ objects.
 *
 * @example
 * auto parser = ArgumentParser()
 * parser.AddArgument("--foo").WithDefault(100).Required();
 * // --foo=200
 * auto args = parser.Parse(argc, argv);
 * assert(args["foo"].AsLong() == 200);
 */
class ArgumentParser {
 public:
  using Arguments = std::map<std::string, Argument>;

  explicit ArgumentParser(std::string_view prog) : prog_(prog) {
    args_.emplace_back("--help");
    args_.back().StoreTrue().WithHelp("Show this message");
  }

  ArgumentParser() = default;
  ~ArgumentParser() = default;

  /**
   * @brief Add a new argument definition
   *
   * @param arg: name of the argument, patterns can be:
   *    - "foo": a position argument
   *    - "--foo": a keyword argument or a flag
   * @return Argument& reference of the argument for further settings.
   */
  Argument& AddArgument(std::string_view arg) {
    if (arg.empty()) {
      throw ParseError("arg can not be empty!");
    }
    if (arg[0] == '-') {
      args_.emplace_back(arg);
      checkin(args_.back().name_);
      return args_.back();
    }
    pos_args_.emplace_back(arg);
    pos_args_.back().pos_ = static_cast<int64_t>(pos_args_.size() - 1);
    checkin(pos_args_.back().name_);
    return pos_args_.back();
  }

  /**
   * @brief Parse command line to known arguments
   *
   * @param argc: counts of args
   * @param argv: value of args
   * @return a tuple of two maps, where the 1st one is the known arguments
   *   and the second one is the unknown arguments.
   */
  std::tuple<Arguments, std::map<std::string, std::string>> ParseKnown(
      int argc, const char* const* argv
  ) {
    Arguments args;
    std::map<std::string, std::string> unknown_args;
    for (auto& known_args : args_) {
      args[known_args.name_] = known_args;
    }
    for (auto& known_args : pos_args_) {
      args[known_args.name_] = known_args;
    }
    auto raw_args = parseArgs(argc, argv);
    for (auto&& [key, value] : raw_args) {
      if (args.count(key)) {
        args[key].setValue(value);
      } else {
        unknown_args[key] = value;
      }
    }
    std::string err_msg;
    for (auto&& [key, value] : args) {
      value.check(err_msg);
    }
    if (err_msg.empty()) {
      return {args, unknown_args};
    }
    PrintHelp();
    throw ParseError(err_msg);
  }

  Arguments ParseDict(const std::map<std::string, std::string>& dict) {
    std::vector<std::string> args{"__PROGRAM__"};
    for (auto&& [key, value] : dict) {
      if (key.empty() || key == "__POS__") {
        if (!value.empty()) {
          for (const auto& v : StrSplit(value, ";")) {
            args.emplace_back(v);
          }
        }
        continue;
      }
      args.emplace_back("--" + key);
      if (!value.empty()) {
        args.emplace_back(value);
      }
    }
    const char** argv = new const char*[args.size()];
    int i = 0;
    for (const auto& arg : args) {
      argv[i++] = arg.c_str();
    }
    std::string error;
    try {
      return Parse(static_cast<int>(args.size()), argv);
    } catch (const std::exception& ex) {
      error = ex.what();
    }
    delete[] argv;
    throw ParseError(error);
  }

  /**
   * @brief Parse the command line to argument map.
   *
   * @throw ParseError if unknown arguments is given
   *
   * @param argc: counts of args
   * @param argv: value of args
   * @return Arguments the map of defined arguments
   */
  Arguments Parse(int argc, const char* const* argv) {
    auto&& [args, unkonwn_args] = ParseKnown(argc, argv);
    if (!args["help"].None()) {
      PrintHelp();
      exit(0);  // NOLINT(concurrency-*)
    }
    if (unkonwn_args.empty()) {
      return args;
    }
    std::string unknown;
    for (auto& arg : unkonwn_args) {
      unknown += arg.first + " ";
    }
    PrintHelp();
    throw ParseError("unknown args: " + unknown);
  }

  /** @brief Print the auto generated help message. */
  void PrintHelp() const {
    std::cout << prog_ << " ";
    for (const auto& arg : pos_args_) {
      std::cout << "<" << arg.name_ << "> ";
    }
    for (const auto& arg : args_) {
      if (arg != "help") {
        std::string msg = arg.full_name_;
        if (!arg.IsFlag()) {
          msg += " ...";
        }
        if (arg.IsRequired()) {
          std::cout << msg << " ";
        } else {
          std::cout << "[" << msg << "] ";
        }
      }
    }
    std::cout << std::endl;
    for (const auto& arg : pos_args_) {
      std::cout << "  " << arg.full_name_ << ": " << arg.help_ << std::endl;
    }
    for (const auto& arg : args_) {
      std::cout << "  " << arg.full_name_ << ": " << arg.help_ << std::endl;
    }
    std::cout << std::endl;
  }

 private:
  /** @brief parse command line to raw string map. */
  std::multimap<std::string, std::string> parseArgs(
      int argc, const char* const* argv
  ) {
    // use multimap to take repeated arguments
    std::multimap<std::string, std::string> argopt;

    auto read = [](std::string_view argvalue, const std::string& nextvalue) {
      // split two patterns: "--foo=bar", "--foo bar"
      auto arg_split_equal = StrSplit(argvalue, "=");
      if (arg_split_equal.size() == 2) {
        return std::make_pair(arg_split_equal[0], arg_split_equal[1]);
      } else {
        return std::make_pair(arg_split_equal[0], nextvalue);
      }
    };

    if (prog_.empty()) {
      prog_ = argv[0];
    }

    size_t position = 0;
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg.size() > 2 && arg.substr(0, 2) == "--") {
        auto kv = read(arg.substr(2), i + 1 == argc ? "" : argv[i + 1]);
        argopt.insert(kv);
      } else if (arg.size() > 1 && arg[0] == '-') {
        auto kv = read(arg.substr(1), i + 1 == argc ? "" : argv[i + 1]);
        argopt.insert(kv);
      } else if (arg != "-" && arg != "--" && position < pos_args_.size()) {
        argopt.insert(std::make_pair(pos_args_[position].name_, arg));
        position++;
      }
    }
    // check number of position arguments
    if (position != pos_args_.size()) {
      PrintHelp();
      std::string err_msg;
      for (size_t i = position; i < pos_args_.size(); i++) {
        err_msg += pos_args_[i].name_ + ", ";
      }
      throw ParseError(err_msg + "is not specified");
    }
    return argopt;
  }

  void checkin(const std::string& key) {
    if (uniq_key_.count(key) > 0) {
      throw ParseError("duplicate argument " + key);
    }
    uniq_key_.insert(key);
  }

  std::string prog_;
  std::unordered_set<std::string> uniq_key_;
  std::vector<Argument> args_;
  std::vector<Argument> pos_args_;
};
#endif  // CLIM_ARGPARSE_H_
