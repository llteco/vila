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
 * Description: route algorithms for graph
 ****************************************/
#ifndef VILA_GRAPH_ROUTE_H_
#define VILA_GRAPH_ROUTE_H_
#include <algorithm>
#include <deque>
#include <iterator>
#include <limits>
#include <stack>
#include <unordered_set>

#include "vila/graph/digraph.h"

namespace vila {
/**
 * @brief Check if given `nodes` sequence forms a simple path in graph `g`.
 *
 * A simple path is a sequence of node if and only if each one appears only once
 * and the neighbor node has an edge in the graph.
 * @param g: a graph or a directed graph
 * @param nodes: a container of node indices, order matters.
 * @return (bool) true if `nodes` forms a simple path.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>,
    class Cont = std::deque<typename NodeIndex::index_type>>
bool IsSimplePath(
    const Graph<T, Prop, TNode, NodeIndex>& g, const Cont& nodes
) {
  using Index = typename NodeIndex::index_type;
  std::unordered_set<Index> nodeset(std::begin(nodes), std::end(nodes));
  if (nodeset.empty()) return false;

  for (auto i = std::begin(nodes), j = i + 1; j != std::end(nodes); ++i, ++j) {
    auto adj = g.Adjacent(*i);
    if (std::find(adj.begin(), adj.end(), *j) == adj.end()) {
      return false;
    }
  }
  return nodeset.size() == std::distance(std::begin(nodes), std::end(nodes));
}

/**
 * @brief Get all simple paths in the graph `g` from `source` to `target` node.
 *
 * @param g: a graph
 * @param source: start node
 * @param target: end node
 * @param depth_limit: Depth to stop the search, only paths no longer than
 *   depth_limit is returned.
 * @return (std::deque<std::deque<typename NodeIndex::index_type>>) a list of
 * path contains the sequence of node index.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto AllSimplePaths(
    const Graph<T, Prop, TNode, NodeIndex>& g,
    const typename NodeIndex::index_type& source,
    const typename NodeIndex::index_type& target,
    size_t depth_limit = std::numeric_limits<size_t>::max()
) -> std::deque<std::deque<typename NodeIndex::index_type>> {
  using Index = typename NodeIndex::index_type;
  using Path = std::deque<Index>;
  std::deque<Path> paths;
  // nodes not in the graph
  if (!g.contains(source) || !g.contains(target)) return paths;

  std::stack<Path> stack;
  stack.push(g.Adjacent(source));
  Path path{source};
  while (!stack.empty()) {
    auto& neighbors = stack.top();
    if (neighbors.empty()) {
      stack.pop();
      path.pop_back();
    } else if (path.size() < depth_limit) {
      auto thisnode = neighbors.front();
      neighbors.pop_front();
      if (std::find(path.begin(), path.end(), thisnode) != path.end()) {
        // node already in path, maybe a cycle or revisit
        continue;
      }
      path.push_back(thisnode);
      if (thisnode == target) {
        // found one simple path
        paths.push_back(path);
        path.pop_back();
      } else {
        // find on going
        stack.push(g.Adjacent(thisnode));
      }
    } else {
      // reaches the depth limit
      if (std::find(neighbors.begin(), neighbors.end(), target) !=
          neighbors.end()) {
        paths.push_back(path);
        paths.back().push_back(target);
      }
      stack.pop();
      path.pop_back();
    }
  }
  return paths;
}

/**
 * @brief Get all simple paths in the graph `g` starting from `source` and
 * ending at any leaf node.
 *
 * A leaf node is node with zero outdegree.
 *
 * @note this function has undefined behaviour on undirected graphs.
 *
 * @param g: a graph
 * @param source: start node
 * @return (std::deque<std::deque<typename NodeIndex::index_type>>) a list of
 * path contains the sequence of node index.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto AllSimplePaths(
    const DiGraph<T, Prop, TNode, NodeIndex>& g,
    const typename NodeIndex::index_type& source
) -> std::deque<std::deque<typename NodeIndex::index_type>> {
  std::deque<std::deque<typename NodeIndex::index_type>> paths;
  for (const auto& node : g) {
    if (g.OutDegree(node) == 0) {
      auto path_to_node = AllSimplePaths(g, source, node);
      paths.insert(paths.end(), path_to_node.begin(), path_to_node.end());
    }
  }
  return paths;
}

/**
 * @brief Get all simple paths in the graph `g` starting from any root node and
 * ending at any leaf node.
 *
 * A root node is node with zero indegree, and a leaf node is node with zero
 * outdegree.
 *
 * @note this function has undefined behaviour on undirected graphs.
 *
 * @param g: a graph
 * @return (std::deque<std::deque<typename NodeIndex::index_type>>) a list of
 * path contains the sequence of node index.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto AllSimplePaths(const DiGraph<T, Prop, TNode, NodeIndex>& g
) -> std::deque<std::deque<typename NodeIndex::index_type>> {
  std::deque<std::deque<typename NodeIndex::index_type>> paths;
  for (const auto& node : g) {
    if (g.InDegree(node) == 0) {
      auto path_from_node = AllSimplePaths(g, node);
      paths.insert(paths.end(), path_from_node.begin(), path_from_node.end());
    }
  }
  return paths;
}

/**
 * @brief Get all simple paths in the graph `g` starting from any root node and
 * ending at any leaf node.
 *
 * A root node is node with zero indegree, and a leaf node is node with zero
 * outdegree.
 *
 * @note this function has undefined behaviour on undirected graphs.
 *
 * @param g: a graph
 * @return (std::deque<std::deque<typename NodeIndex::index_type>>) a list of
 * path contains the sequence of node index.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto RootToLeafPaths(const DiGraph<T, Prop, TNode, NodeIndex>& g
) -> std::deque<std::deque<typename NodeIndex::index_type>> {
  return AllSimplePaths(g);
}
}  // namespace vila
#endif  // VILA_GRAPH_ROUTE_H_
