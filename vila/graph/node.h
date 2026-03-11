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
