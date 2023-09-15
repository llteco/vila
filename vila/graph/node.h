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
 * Description: Node container for graph
 ****************************************/
#ifndef VILA_GRAPH_NODE_H_
#define VILA_GRAPH_NODE_H_
#include <deque>
#include <memory>
#include <utility>

namespace vila {
/**
 * @brief A node container for instance T.
 *
 * Instance stored as a unique ptr, the node is made uncopiable.
 * @tparam T: type of instance stored in node.
 */
template <class T>
class Node {
 public:
  explicit Node(T* ins) : ins_(ins) {}

  explicit Node(std::unique_ptr<T> ins) : ins_(std::move(ins)) {}

  virtual ~Node() = default;
  Node(Node&&) noexcept = default;
  Node& operator=(Node&&) noexcept = default;
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;

  /// STL style apis
  // NOLINTBEGIN(readability-identifier-naming)
  typename std::unique_ptr<T>::pointer operator->() const noexcept {
    return ins_.operator->();
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  T* get() const { return ins_.get(); }

  // NOLINTEND(readability-identifier-naming)

 private:
  /// @brief instance hold in the node, which should be unique.
  std::unique_ptr<T> ins_;
};
}  // namespace vila
#endif  // VILA_GRAPH_NODE_H_
