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
 * Description: meta arch for directed graph
 ****************************************/
#ifndef VILA_GRAPH_DIGRAPH_H_
#define VILA_GRAPH_DIGRAPH_H_
#include <deque>
#include <string>
#include <utility>

#include "vila/graph/graph.h"

namespace vila {
/**
 * @brief A meta class representing a directed graph.
 *
 * @sa Graph
 *
 * @tparam T: the type of the instance in vertices.
 * @tparam TNode: the type of the node container for instance.
 * @tparam NodeIndex: the method class to index the node.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
class DiGraph : public Graph<T, Prop, TNode, NodeIndex> {
  using PNode = TNode *;
  using CPNode = const TNode *;
  using Index = typename NodeIndex::index_type;
  using GIndex = std::string;

 public:
  /**
   * @brief Construct a new graph with a name and initial hierarchy level.
   *
   * @param name: a name for a graph, this is used to index nested subgraphs.
   *   The name should be unique in same hierarchy level.
   * @param hierarchy: A positive integer representing the hierarchy level of
   *   this graph. The top graph level is 0, greater number indicates lower
   *   level.
   */
  explicit DiGraph(std::string_view name, uint32_t hierarchy = 0)
      : Graph<T, Prop, TNode, NodeIndex>(name, hierarchy) {}

  DiGraph() = default;
  virtual ~DiGraph() = default;
  /// No deep copy allowed for graph, use Graph::ShallowCopy instead
  DiGraph(const DiGraph &) = delete;
  DiGraph &operator=(const DiGraph &) = delete;
  DiGraph(DiGraph &&) noexcept = default;
  DiGraph &operator=(DiGraph &&) noexcept = default;

  /**
   * @brief Add an edge between node n0 and n1.
   *
   * @overload
   * Add new nodes together with their edge.
   *
   * @note
   * In this class the edge is **directed**. Which means n0-n1 is *not*
   * equivalent to n1-n0.
   * @sa Graph::RemoveEdge
   *
   * @param n0: one of the node instance.
   * @param n1: the other node instance.
   */
  void AddEdge(TNode &&n0, TNode &&n1) {
    NodeIndex indexer;
    Index ind0 = indexer(n0);
    Index ind1 = indexer(n1);
    this->AddNode(std::move(n0));
    this->AddNode(std::move(n1));
    AddEdge(ind0, ind1);
  }

  /**
   * @brief Add an edge between node index p0 and p1.
   *
   * @overload
   * The node p0 and p1 must be added to the graph.
   *
   * @note
   * In this class the edge is **directed**. Which means p0-p1 is *not*
   * equivalent to p1-p0.
   * @sa DiGraph::RemoveEdge
   *
   * @param p0: one of the node index.
   * @param p1: the other node index.
   */
  void AddEdge(Index p0, Index p1) {
    if (!this->contains(p0) || !this->contains(p1)) return;

    this->adj_[p0].push_back(p1);
    this->pred_[p1].push_back(p0);
    this->edge_props_[std::make_tuple(p0, p1)];
  }

  /**
   * @brief Remove an edge between node index p0 and p1.
   *
   * If you wish to delete an edge from a graph, you will cache the node index
   * yourself. The default NodeIndex is AddressIndex.
   *
   * @note
   * In this class the edge is **directed**. Which means p0-p1 is *not*
   * equivalent to p1-p0.
   * @sa DiGraph::AddEdge
   *
   * @param p0: one of the node index.
   * @param p1: the other node index.
   */
  void RemoveEdge(Index p0, Index p1) {
    if (!this->contains(p0) || !this->contains(p1)) return;

    auto &v0 = this->adj_[p0];
    v0.erase(
        std::remove_if(
            v0.begin(), v0.end(), [&p1](const Index &i) { return i == p1; }
        ),
        v0.end()
    );
    auto &v1 = this->pred_[p1];
    v1.erase(
        std::remove_if(
            v1.begin(), v1.end(), [&p0](const Index &i) { return i == p0; }
        ),
        v1.end()
    );
    this->edge_props_.erase(std::make_tuple(p0, p1));
  }

  /**
   * @brief Set the property of node index `i`.
   *
   * @param i: the index of node.
   * @param prop: property class of the node to set.
   */
  void SetProperty(Index i, Prop prop) {
    if (!this->contains(i)) return;

    this->node_props_[i] = std::move(prop);
  }

  /**
   * @brief Set the property of edge (u, v).
   *
   * @param u: startpoint index of edge.
   * @param v: endpoint index of edge.
   * @param prop: property class of the edge to set.
   */
  void SetProperty(Index u, Index v, Prop prop) {
    if (!this->contains(u) || !this->contains(v)) return;

    this->edge_props_[std::make_tuple(u, v)] = prop;
  }

  /**
   * @brief The total number of edges in the graph.
   *
   * @sa Graph::NumNodes
   * @return (size_t) the number of edges.
   */
  size_t NumEdges() const {
    auto di_edges = std::accumulate(
        this->adj_.begin(), this->adj_.end(), 0LLU,
        [](size_t val, typename decltype(this->adj_)::value_type pair) {
          return val + pair.second.size();
        }
    );
    return di_edges;
  }

  /**
   * @brief Get all the successor nodes of node index `n`
   *
   * Successor list represents all neighboring nodes from node `n`, which also
   * means all nodes have edge starting from `n`.
   *
   * @param n: index of node
   * @return (std::deque<Index>) a list of node indices success to `n`.
   */
  std::deque<Index> Successor(Index n) const {
    if (!this->contains(n)) return {};
    return this->adj_.at(n);
  }

  /**
   * @brief Get all the predecessor nodes of node index `n`
   *
   * Predecessor list represents all neighboring nodes to node `n`, which also
   * means all nodes have edge ending at `n`.
   *
   * @param n: index of node
   * @return (std::deque<Index>) a list of node indices predecess to `n`.
   */
  std::deque<Index> Predecessor(Index n) const {
    if (!this->contains(n)) return {};
    return this->pred_.at(n);
  }

  /**
   * @brief Get the indegree of node index `n`.
   *
   * Indegree is the number of input edges (fanin) to `n`.
   *
   * @param n: index of node.
   * @return (int64_t) indegree of node `n`, or -1 if n is not existed.
   */
  int64_t InDegree(Index n) const {
    if (!this->contains(n)) return -1;
    return static_cast<int64_t>(Predecessor(n).size());
  }

  /**
   * @brief Get the outdegree of node index `n`.
   *
   * Outdegree is the number of output edges (fanout) from `n`
   *
   * @param n: index of node.
   * @return (int64_t) outdegree of node `n`, or -1 if n is not existed.
   */
  int64_t OutDegree(Index n) const {
    if (!this->contains(n)) return -1;
    return static_cast<int64_t>(Successor(n).size());
  }

  /**
   * @brief Reverse the direction of each edge in this graph.
   */
  void Reverse() { std::swap(this->pred_, this->adj_); }

  /**
   * @brief Make a shallow copy of the graph, node instance is not copied.
   *
   * This function copies just node indices and edges of the graph, it's used
   * for graph algorithms, uses shall never use the shallow copied graph.
   * @return (DiGraph) a copy of the graph.
   */
  DiGraph ShallowCopy() const { return this->template copyHelper<DiGraph>(); }

  /**
   * @brief Generate a shallow copy of subgraph of nodes in range(beg, end)
   *
   * Subgraph only contains nodes and edges, no hierarchical data.
   *
   * @tparam It: iterator of node index.
   * @param beg: begin of the index iterator.
   * @param end: end of the index iterator.
   * @return (DiGraph) a copy of the subgraph.
   */
  template <class It>
  DiGraph Subgraph(It beg, It end) const {
    return this->template subgraphHelper<DiGraph>(beg, end);
  }

  /**
   * @brief Generate a shallow copy of subgraph of nodes in container.
   *
   * Subgraph only contains nodes and edges, no hierarchical data.
   *
   * @tparam Cont: type of the STL-like containers.
   * @param cont: the node index container.
   * @return (DiGraph) a copy of the subgraph.
   */
  template <class Cont>
  DiGraph Subgraph(Cont cont) const {
    return this->template subgraphHelper<DiGraph>(
        std::begin(cont), std::end(cont)
    );
  }
};
}  // namespace vila
#endif  // VILA_GRAPH_DIGRAPH_H_
