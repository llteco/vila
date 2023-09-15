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
 * Description: traversal methods for graph
 ****************************************/
#ifndef VILA_GRAPH_TRAVERSAL_H_
#define VILA_GRAPH_TRAVERSAL_H_
#include <algorithm>
#include <deque>
#include <iterator>
#include <limits>
#include <stack>
#include <tuple>
#include <unordered_set>

#include "vila/graph/digraph.h"

namespace vila {
/**
 * @brief Iterate over edges in a depth-first-search (DFS).
 *   Perform a depth-first-search over the nodes of `G` and yield
 *   the edges in order. This may not generate all edges in `G`
 *   (see `~networkx.algorithms.traversal.edgedfs.edge_dfs`).
 *
 * @note This algorithm is referenced from networkx. See
 * https://github.com/networkx/networkx
 *
 * @sa DiGraph
 * @param g: a DiGraph to apply dfs
 * @param sources: a set of node indices specifying starting node for
 *   depth-first search and return edges in the component reachable from
 *   sources.
 * @param depth_limit: Specify the maximum search depth.
 * @return (std::deque<std::tuple<NodeIndex, NodeIndex>>) a list of edge tuple
 *   (u, v) in the dfs order.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto DfsEdges(
    const DiGraph<T, Prop, TNode, NodeIndex>& g,
    std::unordered_set<typename NodeIndex::index_type> sources,
    size_t depth_limit = std::numeric_limits<size_t>::max()
)
    -> std::deque<std::tuple<
        typename NodeIndex::index_type,
        typename NodeIndex::index_type>> {
  using Index = typename NodeIndex::index_type;
  using Edge = std::tuple<Index, Index>;

  std::unordered_set<Index> visited;
  std::deque<Edge> edges;
  for (auto& node : sources) {
    if (visited.count(node)) continue;

    visited.insert(node);
    std::stack<std::tuple<Index, size_t, std::deque<Index>>> stack;
    stack.push(std::make_tuple(node, depth_limit, g.Successor(node)));
    while (!stack.empty()) {
      auto&& [parent, depth_now, children] = stack.top();
      if (children.empty()) {
        stack.pop();
        continue;
      }
      auto child = children.front();
      children.pop_front();
      if (!visited.count(child)) {
        visited.insert(child);
        edges.emplace_back(std::make_tuple(parent, child));
        if (depth_now > 1) {
          stack.push(std::make_tuple(child, depth_now - 1, g.Successor(child)));
        }
      }
    }
  }
  return edges;
}

/**
 * @brief Yield all edges is dfs order.
 * @overload DfsEdges
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto DfsEdges(const DiGraph<T, Prop, TNode, NodeIndex>& g) {
  return DfsEdges<T, TNode, NodeIndex>(g, g.GetNodes(), g.NumNodes());
}

/**
 * @brief Types of labeled edges (*u*, *v*), which is one of the 'forward',
 *   'nontree', or 'reverse'. A 'forward' edge is one in which *u* has been
 *   visited but *v* has not. A 'nontree' edge is one in which both *u* and *v*
 *   have been visited but the edge is not in the DFS tree. A 'reverse' edge is
 *   on in which both *u* and *v* have been visited and the edge is in the DFS
 *   tree.
 */
enum class edge_label { forward, nontree, reverse };

/**
 * @brief Iterate over edges in a depth-first-search (DFS).
 *   Perform a depth-first-search over the nodes of `G` and yield
 *   the edges in order. The labels reveal the complete transcript of the
 *   depth-first search algorithm in more detail than DfsEdges.
 *
 * @note This algorithm is referenced from networkx. See
 * https://github.com/networkx/networkx
 *
 * @sa DiGraph
 * @param g: a DiGraph to apply dfs
 * @param sources: a set of node indices specifying starting node for
 *   depth-first search and return edges in the component reachable from
 *   sources.
 * @param depth_limit: Specify the maximum search depth.
 * @return (std::deque<std::tuple<U, U, edge_label>>) a list of edge tuple
 *   (u, v, label) in the dfs order.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto DfsLabeledEdges(
    const DiGraph<T, Prop, TNode, NodeIndex>& g,
    std::unordered_set<typename NodeIndex::index_type> sources,
    size_t depth_limit = std::numeric_limits<size_t>::max()
)
    -> std::deque<std::tuple<
        typename NodeIndex::index_type,
        typename NodeIndex::index_type,
        edge_label>> {
  using Index = typename NodeIndex::index_type;
  using LabeledEdge = std::tuple<Index, Index, edge_label>;

  std::unordered_set<Index> visited;
  std::deque<LabeledEdge> edges;
  for (auto& start : sources) {
    if (visited.count(start)) continue;

    visited.insert(start);
    edges.push_back(std::make_tuple(start, start, edge_label::forward));
    std::stack<std::tuple<Index, size_t, std::deque<Index>>> stack;
    stack.push(std::make_tuple(start, depth_limit, g.Successor(start)));
    while (!stack.empty()) {
      auto&& [parent, depth_now, children] = stack.top();
      if (children.empty()) {
        stack.pop();
        if (!stack.empty()) {
          edges.push_back(std::make_tuple(
              std::get<0>(stack.top()), parent, edge_label::reverse
          ));
        }
        continue;
      }
      auto child = children.front();
      children.pop_front();
      if (visited.count(child)) {
        edges.push_back(std::make_tuple(parent, child, edge_label::nontree));
      } else {
        visited.insert(child);
        edges.push_back(std::make_tuple(parent, child, edge_label::forward));
        if (depth_now > 1) {
          stack.push(std::make_tuple(child, depth_now - 1, g.Successor(child)));
        }
      }
    }
    edges.push_back(std::make_tuple(start, start, edge_label::reverse));
  }
  return edges;
}

/**
 * @brief Yield all edges is dfs order.
 * @overload DfsLabeledEdges
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto DfsLabeledEdges(const DiGraph<T, Prop, TNode, NodeIndex>& g) {
  return DfsLabeledEdges(g, g.GetNodes(), g.NumNodes());
}

/**
 * @brief Generate nodes in a depth-first-search pre-ordering starting at
 * sources.
 *
 * @param g: a DiGraph to apply dfs.
 * @param sources: a set of node indices specifying starting node for
 *   depth-first search and return edges in the component reachable from
 *   sources.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in pre-ordering sequence.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> DfsPreorder(
    const DiGraph<T, Prop, TNode, NodeIndex>& g,
    std::unordered_set<typename NodeIndex::index_type> sources
) {
  auto labeled_edges = DfsLabeledEdges(g, sources);
  std::deque<typename NodeIndex::index_type> preorder;
  for (auto&& [u, v, label] : labeled_edges) {
    if (label == edge_label::forward) {
      preorder.push_back(v);
    }
  }
  return preorder;
}

/**
 * @brief Generate nodes in a depth-first-search pre-ordering starting at
 * an arbitrary source.
 *
 * @param g: a DiGraph to apply dfs.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in pre-ordering sequence.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> DfsPreorder(
    const DiGraph<T, Prop, TNode, NodeIndex>& g
) {
  auto labeled_edges = DfsLabeledEdges(g);
  std::deque<typename NodeIndex::index_type> preorder;
  for (auto&& [u, v, label] : labeled_edges) {
    if (label == edge_label::forward) {
      preorder.push_back(v);
    }
  }
  return preorder;
}

/**
 * @brief Generate nodes in a depth-first-search post-ordering starting at
 * sources.
 *
 * @param g: a DiGraph to apply dfs.
 * @param sources: a set of node indices specifying starting node for
 *   depth-first search and return edges in the component reachable from
 *   sources.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in post-ordering sequence.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> DfsPostorder(
    const DiGraph<T, Prop, TNode, NodeIndex>& g,
    std::unordered_set<typename NodeIndex::index_type> sources
) {
  auto labeled_edges = DfsLabeledEdges(g, sources);
  std::deque<typename NodeIndex::index_type> postorder;
  for (auto&& [u, v, label] : labeled_edges) {
    if (label == edge_label::reverse) {
      postorder.push_back(v);
    }
  }
  return postorder;
}

/**
 * @brief Generate nodes in a depth-first-search post-ordering starting at
 * an arbitrary source.
 *
 * @param g: a DiGraph to apply dfs.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in post-ordering sequence.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> DfsPostorder(
    const DiGraph<T, Prop, TNode, NodeIndex>& g
) {
  auto labeled_edges = DfsLabeledEdges(g);
  std::deque<typename NodeIndex::index_type> postorder;
  for (auto&& [u, v, label] : labeled_edges) {
    if (label == edge_label::reverse) {
      postorder.push_back(v);
    }
  }
  return postorder;
}

/**
 * @brief Iterate over edges in a breadth-first-search (BFS).
 *   Perform a breadth-first-search over the nodes of `G` and yield
 *   the edges in order. This may not generate all edges in `G`.
 *
 * @note This algorithm is referenced from networkx. See
 * https://github.com/networkx/networkx
 * @note BFS is applicable to an undirected graph, while DFS can only be
 *   applicable to a directed graph.
 *
 * @sa Graph, DiGraph
 * @param g: a Graph to apply bfs
 * @param sources: a set of node indices specifying starting node for
 *   depth-first search and return edges in the component reachable from
 *   sources.
 * @param depth_limit: Specify the maximum search depth.
 * @return (std::deque<std::tuple<NodeIndex, NodeIndex>>) a list of edge tuple
 *   (u, v) in the bfs order.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto BfsEdges(
    const Graph<T, Prop, TNode, NodeIndex>& g,
    std::unordered_set<typename NodeIndex::index_type> sources,
    size_t depth_limit = std::numeric_limits<size_t>::max()
)
    -> std::deque<std::tuple<
        typename NodeIndex::index_type,
        typename NodeIndex::index_type>> {
  using Index = typename NodeIndex::index_type;
  using Edge = std::tuple<Index, Index>;

  std::unordered_set<Index> visited;
  std::deque<Edge> edges;
  for (auto& node : sources) {
    if (visited.count(node)) continue;

    visited.insert(node);
    std::deque<std::tuple<Index, size_t, std::deque<Index>>> fifo;
    fifo.push_back(std::make_tuple(node, depth_limit, g.Adjacent(node)));
    while (!fifo.empty()) {
      auto&& [parent, depth_now, children] = fifo.front();
      for (const auto& child : children) {
        if (!visited.count(child)) {
          visited.insert(child);
          edges.emplace_back(std::make_tuple(parent, child));
          if (depth_now > 1) {
            fifo.push_back(
                std::make_tuple(child, depth_now - 1, g.Adjacent(child))
            );
          }
        }
      }
      fifo.pop_front();
    }
  }
  return edges;
}

/**
 * @brief Yield all edges is bfs order.
 * @overload BfsEdges
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto BfsEdges(const Graph<T, Prop, TNode, NodeIndex>& g) {
  return BfsEdges(g, g.GetNodes(), g.NumNodes());
}
}  // namespace vila
#endif  // VILA_GRAPH_TRAVERSAL_H_
