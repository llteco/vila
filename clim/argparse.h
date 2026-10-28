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
#ifndef CLIM_ARGPARSE_H_
#define CLIM_ARGPARSE_H_
#include <algorithm>
#include <any>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
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

  template <typename T>
  Argument& WithDefault(T default_value) {
    if constexpr (std::is_integral_v<T>) {
      def_ = static_cast<int64_t>(default_value);
    } else if constexpr (std::is_floating_point_v<T>) {
      def_ = static_cast<float>(default_value);
    } else {
      def_ = std::string(std::move(default_value));
    }
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

  /**
   * @brief Mark the argument repeatable (python argparse action="append"):
   *   repeated occurrences accumulate their values instead of failing the
   *   exact-count check. Nargs still governs the values each single
   *   occurrence takes (default 1); Required demands that minimum count.
   */
  Argument& Append() {
    append_ = true;
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
    } else if (static_cast<char>(nargs) == '?') {
      nargs_ = -2;
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
      if (append_) {
        if (required_ && vals_.size() < static_cast<size_t>(nargs_)) {
          ss << name_ << " expects at least " << nargs_ << " values but gets "
             << vals_.size();
        }
      } else if (vals_.size() != static_cast<size_t>(nargs_)) {
        if ((vals_.empty() && required_) || !vals_.empty()) {
          ss << name_ << " expects " << nargs_ << " values but gets " << vals_.size();
          if (full_name_[0] != '-') {
            ss << "\n"
               << "can not specify position argument with --" << name_;
          }
        }
      }
    } else if (nargs_ == 0 && vals_.empty() && required_) {
      ss << name_ << " expects at least one value but gets none";
    } else if (nargs_ == -2 && vals_.size() > 1) {
      ss << name_ << " expects at most one value but gets " << vals_.size();
    }
    if (!ss.str().empty()) {
      err_msg += ss.str() + "\n";
      return false;
    }
    return true;
  }

  bool required_ = false;
  bool append_ = false;
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
 * A lone "--" terminates the option list: the remaining arguments are
 * always taken as position arguments (e.g. paths starting with "-").
 *
 * Position arguments are filled left-to-right from the non-option tokens
 * and support Nargs (N, '+', '*', '?') like python argparse. Option and
 * position arguments may be interleaved anywhere on the command line.
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
      check(args_.back().name_);
      return args_.back();
    }
    pos_args_.emplace_back(arg);
    check(pos_args_.back().name_);
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
    std::vector<std::string> extras;
    for (auto& known_args : args_) {
      args[known_args.name_] = known_args;
    }
    for (auto& known_args : pos_args_) {
      args[known_args.name_] = known_args;
    }
    auto raw_args = parseArgs(argc, argv, extras);
    for (auto&& [key, value] : raw_args) {
      if (args.count(key)) {
        args[key].setValue(value);
      } else {
        unknown_args[key] = value;
      }
    }
    // leftover positional values are unknown to the parser
    for (auto& extra : extras) {
      unknown_args[extra] = "";
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
      std::string msg = "<" + arg.name_ + ">";
      if (maxNargs(arg) != 1) {
        msg += " ...";
      }
      std::cout << msg << " ";
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
  /** @brief parse command line to raw string map. Extra positional values
   * that no position argument claims are appended to extras. */
  std::multimap<std::string, std::string> parseArgs(
      int argc, const char* const* argv, std::vector<std::string>& extras
  ) {
    // use multimap to take repeated arguments
    std::multimap<std::string, std::string> argopt;

    if (prog_.empty()) {
      prog_ = argv[0];
    }

    auto find_arg = [this](std::string_view key) -> const Argument* {
      for (const auto& arg : args_) {
        if (arg.name_ == key) {
          return &arg;
        }
      }
      return nullptr;
    };
    // a token can only be taken as an option value if it does not look
    // like an option itself ("-" alone is a value, e.g. stdin)
    auto is_value = [](std::string_view tok) {
      return tok.size() == 1 || tok[0] != '-';
    };

    std::vector<std::string> positions;
    // a lone "--" ends the option list; only the first one is a separator
    bool end_of_options = false;
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (!end_of_options && arg == "--") {
        end_of_options = true;
        continue;
      }
      if (!end_of_options && arg.size() > 1 && arg[0] == '-') {
        // two patterns: "--foo=bar" and "--foo bar"
        std::string_view full = arg;
        bool two_dashes = full.substr(0, 2) == "--";
        std::string_view body = two_dashes ? full.substr(2) : full.substr(1);
        auto eq = body.find('=');
        if (eq != std::string_view::npos) {
          std::string key(body.substr(0, eq));
          std::string value(body.substr(eq + 1));
          argopt.insert(std::make_pair(key, value));
          continue;
        }
        const Argument* known = find_arg(body);
        if (!known || known->IsFlag()) {
          // flags take no value; unknown options are reported as-is and
          // keep their following tokens available as position arguments
          argopt.insert(std::make_pair(std::string(body), ""));
          continue;
        }
        // greedily take the following tokens as values, "--foo a b"
        int64_t remaining = maxNargs(*known);
        while (remaining != 0 && i + 1 < argc && is_value(argv[i + 1])) {
          argopt.insert(std::make_pair(std::string(body), argv[i + 1]));
          i++;
          if (remaining > 0) {
            remaining--;
          }
        }
        continue;
      }
      positions.emplace_back(arg);
    }

    // distribute the collected values to position arguments, reserving the
    // minimum number of values each remaining position argument needs
    std::vector<int64_t> reserved(pos_args_.size() + 1, 0);
    for (size_t k = pos_args_.size(); k-- > 0;) {
      reserved[k] = reserved[k + 1] + minNargs(pos_args_[k]);
    }
    size_t next = 0;
    for (size_t k = 0; k < pos_args_.size(); k++) {
      int64_t left = static_cast<int64_t>(positions.size() - next);
      int64_t avail = left - reserved[k + 1];
      if (avail < minNargs(pos_args_[k])) {
        PrintHelp();
        std::string err_msg;
        for (size_t j = k; j < pos_args_.size(); j++) {
          err_msg += pos_args_[j].name_ + ", ";
        }
        throw ParseError(err_msg + "is not specified");
      }
      int64_t cap = maxNargs(pos_args_[k]);
      int64_t take = cap < 0 ? avail : std::min(cap, avail);
      for (int64_t t = 0; t < take; t++) {
        argopt.insert(std::make_pair(pos_args_[k].name_, positions[next++]));
      }
    }
    for (; next < positions.size(); next++) {
      extras.emplace_back(positions[next]);
    }
    return argopt;
  }

  /** @brief minimum number of values an argument must take. */
  static int64_t minNargs(const Argument& arg) {
    if (arg.nargs_ == 0) return 1;  // '+'
    if (arg.nargs_ < 0) return 0;   // '*' and '?'
    return arg.nargs_;
  }

  /** @brief maximum number of values an argument can take; a negative
   * maximum means unlimited. */
  static int64_t maxNargs(const Argument& arg) {
    if (arg.nargs_ == 0 || arg.nargs_ == -1) return -1;  // '+' and '*'
    if (arg.nargs_ == -2) return 1;                      // '?'
    return arg.nargs_;
  }

  void check(const std::string& key) {
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
