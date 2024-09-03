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
 * Description: node property
 ****************************************/
#ifndef VILA_GRAPH_PROPERTY_H_
#define VILA_GRAPH_PROPERTY_H_
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <deque>
#include <string>

namespace vila::node {
/// @brief appearance property for visualization
enum class color : int { none, black, red, green, blue, yellow, cyan, gray };

/// @brief appearance property for visualization
enum class shape : int { circle, ellipse, rect, diamond, octagon, cylinder };

/// @brief appearance property for visualization
enum class style : int {
  filled = 0x1,
  invisible = 0x2,
  diagonals = 0x4,
  rounded = 0x8,
  dashed = 0x10,
  dotted = 0x20,
  solid = 0x40,
  bold = 0x80
};

constexpr int operator&(style a, style b) {
  return static_cast<int>(a) & static_cast<int>(b);
}

constexpr style operator|(style a, style b) {
  return static_cast<style>(static_cast<int>(a) | static_cast<int>(b));
}
}  // namespace vila::node

/**
 * @brief Formatter in {fmt} for type `node::color`
 *
 * This is a general template formatter for type `node::color`, inherited
 * from `basic_string_view<Char>`, so the format specifications such as {:}
 * follows the rules of basic string.
 * One method `format` must be implemented.
 * @note See https://fmt.dev/9.0.0/api.html#format-api
 * @example
 * fmt::format("{}", node::color::red);  // "red"
 */
template <typename Char>
struct fmt::formatter<vila::node::color, Char>
    : fmt::formatter<fmt::basic_string_view<Char>> {
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-*) ignore function naming style
  auto format(const vila::node::color& color, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    switch (color) {
      case vila::node::color::none:
        return fmt::format_to(ctx.out(), "none");
      case vila::node::color::black:
        return fmt::format_to(ctx.out(), "black");
      case vila::node::color::red:
        return fmt::format_to(ctx.out(), "red");
      case vila::node::color::green:
        return fmt::format_to(ctx.out(), "green");
      case vila::node::color::blue:
        return fmt::format_to(ctx.out(), "blue");
      case vila::node::color::yellow:
        return fmt::format_to(ctx.out(), "yellow");
      case vila::node::color::cyan:
        return fmt::format_to(ctx.out(), "cyan");
      case vila::node::color::gray:
        return fmt::format_to(ctx.out(), "gray");
      default:
        return fmt::format_to(ctx.out(), "unknown");
    }
  }
};

/**
 * @brief Formatter in {fmt} for type `node::shape`
 *
 * This is a general template formatter for type `node::shape`, inherited
 * from `basic_string_view<Char>`, so the format specifications such as {:}
 * follows the rules of basic string.
 * One method `format` must be implemented.
 * @note See https://fmt.dev/9.0.0/api.html#format-api
 * @example
 * fmt::format("{}", node::shape::rect);  // "rect"
 */
template <typename Char>
struct fmt::formatter<vila::node::shape, Char>
    : fmt::formatter<fmt::basic_string_view<Char>> {
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-*) ignore function naming style
  auto format(const vila::node::shape& shape, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    switch (shape) {
      case vila::node::shape::circle:
        return fmt::format_to(ctx.out(), "circle");
      case vila::node::shape::cylinder:
        return fmt::format_to(ctx.out(), "cylinder");
      case vila::node::shape::diamond:
        return fmt::format_to(ctx.out(), "diamond");
      case vila::node::shape::ellipse:
        return fmt::format_to(ctx.out(), "ellipse");
      case vila::node::shape::octagon:
        return fmt::format_to(ctx.out(), "octagon");
      case vila::node::shape::rect:
        return fmt::format_to(ctx.out(), "rect");
      default:
        return fmt::format_to(ctx.out(), "unknown");
    }
  }
};

/**
 * @brief Formatter in {fmt} for type `node::style`
 *
 * This is a general template formatter for type `node::style`, inherited
 * from `basic_string_view<Char>`, so the format specifications such as {:}
 * follows the rules of basic string.
 * One method `format` must be implemented.
 * @note See https://fmt.dev/9.0.0/api.html#format-api
 * @example
 * fmt::format("{}", node::style::rect);  // "rect"
 */
template <typename Char>
struct fmt::formatter<vila::node::style, Char>
    : fmt::formatter<fmt::basic_string_view<Char>> {
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-*) ignore function naming style
  auto format(const vila::node::style& style, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    std::deque<std::string> strs;
    if (style & vila::node::style::filled) strs.emplace_back("filled");
    if (style & vila::node::style::invisible) strs.emplace_back("invisible");
    if (style & vila::node::style::diagonals) strs.emplace_back("diagonals");
    if (style & vila::node::style::rounded) strs.emplace_back("rounded");
    if (style & vila::node::style::dashed) strs.emplace_back("dashed");
    if (style & vila::node::style::dotted) strs.emplace_back("dotted");
    if (style & vila::node::style::solid) strs.emplace_back("solid");
    if (style & vila::node::style::bold) strs.emplace_back("bold");
    return fmt::format_to(ctx.out(), "{}", fmt::join(strs, ","));
  }
};

#endif  // VILA_GRAPH_PROPERTY_H_
