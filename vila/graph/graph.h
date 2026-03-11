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
 * Description: Normal (undirected) graph
 ****************************************/
#ifndef VILA_GRAPH_GRAPH_H_
#define VILA_GRAPH_GRAPH_H_
#include <fmt/format.h>

#include <algorithm>
#include <deque>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "clim/str_split.h"
#include "vila/graph/node.h"

/// @brief A hash function for std::tuple<T, U>
template <class T, class U>
struct std::hash<std::tuple<T, U>> {
  size_t operator()(const std::tuple<T, U> &t) const noexcept {
    constexpr size_t kPrime = 0x9e3779b9LLU;
    size_t seed = std::hash<T>()(std::get<0>(t));
    seed ^= std::hash<U>()(std::get<1>(t)) + kPrime + (seed << 6) + (seed >> 2);
    return seed;
  }
};

namespace vila {
template <class T, class TNode = Node<T>>
struct AddressIndex {
  using index_type = T *;

  index_type operator()(const TNode &node) const { return node.get(); }
};

/// @brief An empty property predefined
struct DummyProperty {};

/**
 * @brief A helper to call copy construction of object T.
 *
 * @param obj
 * @return a copy of obj
 */
template <class T, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
T CopyConstruct(const T &obj) {
  return T(obj);
}

/**
 * @brief A meta class representing a general graph.
 *
 * Vertices in the graph is called *Node*, and a node contains an instance of
 * object T. The node is indexed by NodeIndex class which should be a unique
 * and hashable type for each instance T.
 * The index method NodeIndex should be a class that provides `index_type`
 * type information and an overload of operator() to return the `index_type`.
 *
 * @tparam T: the type of the instance in vertices.
 * @tparam Prop: the type of a property class for each node.
 * @tparam TNode: the type of the node container for instance.
 * @tparam NodeIndex: the method class to index the node.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
class Graph {
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
  explicit Graph(std::string_view name, uint32_t hierarchy = 0)
      : hierarchy_(hierarchy) {
    name_ = name;
  }

  Graph() = default;
  virtual ~Graph() = default;
  /// No deep copy allowed for graph, use Graph::ShallowCopy instead
  Graph(const Graph &) = delete;
  Graph &operator=(const Graph &) = delete;
  Graph(Graph &&) noexcept = default;
  Graph &operator=(Graph &&) noexcept = default;

  /**
   * @brief Get the index list of the node.
   *
   * @return (std::deque<Index>) a deque of node indices
   */
  std::deque<Index> GetNodeDeque() const {
    std::deque<Index> indices;
    NodeIndex indexer;
    for (const auto &n : nodes_) {
      indices.emplace_back(indexer(n));
    }
    for (auto &&[_, subg] : subgraphs_) {
      auto sub_raw = subg->GetNodes();
      indices.insert(indices.end(), sub_raw.begin(), sub_raw.end());
    }
    return indices;
  }

  /**
   * @brief Get the index list of the node.
   *
   * @return (std::unordered_set<Index>) a hash table of node indices
   */
  std::unordered_set<Index> GetNodes() const { return uniq_; }

  /**
   * @brief Add a new node to the graph.
   *
   * User should transfer the ownership of the node into the graph by calling
   * std::move.
   * @example:
   *   g.AddNode(Node<int>(new int));  // from an rvalue
   *   Node<int> a(new int);
   *   g.AddNode(std::move(a));  // from a lvalue
   *
   * @sa Graph::AddNodes, Graph::RemoveNode, Graph::RemoveNodes
   *
   * @param node: a node instance, must be an rvalue.
   */
  void AddNode(TNode &&node) {
    NodeIndex indexer;
    Index ind = indexer(node);
    // node has been added in graph
    if (contains(ind)) return;

    nodes_.push_back(std::move(node));
    uniq_.insert(ind);
    node_props_[ind];  // add default property
    adj_[ind] = {};
    pred_[ind] = {};
  }

  /**
   * @brief Add nodes from an stl container.
   *
   * @sa Graph::AddNode, Graph::RemoveNode, Graph::RemoveNodes
   *
   * @tparam It: the iterator type of the container.
   * @param beg: the start iterator
   * @param end: the end iterator
   */
  template <class It>
  void AddNodes(It beg, It end) {
    static_assert(
        std::is_same_v<typename std::iterator_traits<It>::value_type, TNode>,
        "Iterator value type is not TNode"
    );

    for (auto it = beg; it != end; ++it) AddNode(std::move(*it));
  }

  /**
   * @brief Add nodes from an stl container.
   *
   * @sa Graph::AddNode, Graph::RemoveNode, Graph::RemoveNodes
   *
   * @tparam Cont: the container type.
   * @param cont: the nodes container, must be an rvalue.
   */
  template <class Cont>
  void AddNodes(Cont &&cont) {
    AddNodes(std::begin(cont), std::end(cont));
  }

  /**
   * @brief Remove a node index `i` from the graph.
   *
   * If you wish to delete a node from a graph, you will cache the node index
   * yourself. The default NodeIndex is AddressIndex.
   *
   * @sa Graph::AddNodes, Graph::AddNode, Graph::RemoveNodes
   *
   * @param i: index of the node to remove.
   */
  void RemoveNode(Index i) {
    if (!contains(i)) return;

    // first try to remove in the subgraph
    for (auto &&[_, subg] : subgraphs_) {
      subg->RemoveNode(i);
    }

    // erase in the adj list and uniq list
    uniq_.erase(i);
    node_props_.erase(i);
    for (auto &v : adj_[i]) {
      // remove in the connection list
      this->RemoveEdge(i, v);
    }
    adj_.erase(i);
    NodeIndex indexer;
    nodes_.erase(
        std::remove_if(
            nodes_.begin(), nodes_.end(),
            [&indexer, &i](const TNode &n) { return indexer(n) == i; }
        ),
        nodes_.end()
    );
  }

  /**
   * @brief Remove nodes in an index container.
   *
   * @sa Graph::AddNodes, Graph::AddNode, Graph::RemoveNode
   *
   * @tparam It: the type of the container iterator.
   * @param beg: the start iterator.
   * @param end: the end iterator.
   */
  template <class It>
  void RemoveNodes(It beg, It end) {
    for (auto i = beg; i != end; ++i) {
      RemoveNode(*i);
    }
  }

  /**
   * @brief Add an edge between node n0 and n1.
   *
   * @overload
   * Add new nodes together with their edge.
   *
   * @note
   * In this class the edge is **undirected**. Which means n0-n1 is equivalent
   * to n1-n0.
   * @sa Graph::RemoveEdge
   *
   * @param n0: one of the node instance.
   * @param n1: the other node instance.
   */
  void AddEdge(TNode &&n0, TNode &&n1) {
    NodeIndex indexer;
    Index ind0 = indexer(n0);
    Index ind1 = indexer(n1);
    AddNode(std::move(n0));
    AddNode(std::move(n1));
    AddEdge(ind0, ind1);
  }

  /**
   * @brief Add an edge between node index p0 and p1.
   *
   * @overload
   * The node p0 and p1 must be added to the graph.
   *
   * @note
   * In this class the edge is **undirected**. Which means p0-p1 is equivalent
   * to p1-p0.
   * @sa Graph::RemoveEdge
   *
   * @param p0: one of the node index.
   * @param p1: the other node index.
   */
  void AddEdge(Index p0, Index p1) {
    if (!contains(p0) || !contains(p1)) return;

    adj_[p0].push_back(p1);
    adj_[p1].push_back(p0);
    edge_props_[std::make_tuple(p0, p1)];  // insert default edge property
    edge_props_[std::make_tuple(p1, p0)];
  }

  /**
   * @brief Remove an edge between node index p0 and p1.
   *
   * If you wish to delete an edge from a graph, you will cache the node index
   * yourself. The default NodeIndex is AddressIndex.
   *
   * @note
   * In this class the edge is **undirected**. Which means p0-p1 is equivalent
   * to p1-p0.
   * @sa Graph::AddEdge
   *
   * @param p0: one of the node index.
   * @param p1: the other node index.
   */
  void RemoveEdge(Index p0, Index p1) {
    if (!contains(p0) || !contains(p1)) return;

    auto &v0 = adj_[p0];
    v0.erase(
        std::remove_if(
            v0.begin(), v0.end(), [&p1](const Index &i) { return i == p1; }
        ),
        v0.end()
    );
    auto &v1 = adj_[p1];
    v1.erase(
        std::remove_if(
            v1.begin(), v1.end(), [&p0](const Index &i) { return i == p0; }
        ),
        v1.end()
    );
    edge_props_.erase(std::make_tuple(p1, p0));
    edge_props_.erase(std::make_tuple(p0, p1));
  }

  /**
   * @brief Add a graph as a subgraph in this graph. All nodes and edges in the
   * subgraph is inherited. And the hierarchical information of the subgraph is
   * also kept with its hierarchical level 1 greater than the current graph.
   *
   * User should transfer the ownership of the graph into the graph by calling
   * std::move.
   * @example:
   *   g.AddGraph(Graph("sub"));  // from an rvalue
   *   Graph<int> gg("sub");
   *   g.AddGraph(std::move(gg));  // from a lvalue
   *
   * @note subgraph is indexed by its name (get by `str()` method), nested graph
   * names follow the posix path style. E.g. "main/sub/subsubgraph", where the
   * top graph is "main" and the second level subgraph is "sub", while the 3rd
   * level graph in the subgraph "sub" is "subsubgraph".
   * For convenience the name of the top graph can be omitted, for example name
   * "/sub/subsubgraph" or "sub/subsubgraph" are both OK for indexing.
   *
   * @sa Graph::RemoveGraph
   *
   * @param g: the graph instance to add, must be an rvalue.
   */
  void AddGraph(Graph &&g) {
    GIndex ind = g.str();  // get index before prepending name
    uniq_.merge(CopyConstruct(g.uniq_));
    node_props_.merge(CopyConstruct(g.node_props_));
    edge_props_.merge(CopyConstruct(g.edge_props_));
    adj_.merge(CopyConstruct(g.adj_));
    pred_.merge(CopyConstruct(g.pred_));
    g.IncreaseHierarchy();
    g.PrefixIndex(name_);
    subgraphs_.emplace(
        std::make_pair(ind, std::make_unique<Graph>(std::move(g)))
    );
  }

  /**
   * @brief Remove a subgraph indexed `gind`.
   *
   * All the node and edges of that subgraph will also be removed.
   * Index of the subgraph is a poxis path style string. E.g.
   * "main/sub/subsubgraph", where the top graph is "main" and the second level
   * subgraph is "sub", while the 3rd level graph in the subgraph "sub" is
   * "subsubgraph".
   *
   * @sa Graph::AddGraph
   *
   * @param gind: index of the graph to remove.
   */
  void RemoveGraph(const GIndex &gind) {
    auto paths = StrSplit(gind, "/");
    if (paths.front() == str() || paths.front().empty()) {
      paths.erase(paths.begin());
    }
    // go deep into the subgraph
    auto *gptr = this;
    auto gind_to_remove = paths.back();
    paths.pop_back();
    for (const auto &path : paths) {
      // incorrect index, do nothing
      if (gptr->subgraphs_.count(path) == 0) return;
      gptr = gptr->subgraphs_[path].get();
    }
    if (gptr->subgraphs_.count(gind_to_remove) == 0) return;
    auto nodes_to_del = gptr->subgraphs_[gind_to_remove]->uniq_;
    gptr->subgraphs_.erase(gind_to_remove);
    // remove edges and nodes in adj in each hierarchy
    RemoveNodes(nodes_to_del.begin(), nodes_to_del.end());
    gptr = this;
    for (const auto &path : paths) {
      gptr = gptr->subgraphs_[path].get();
      gptr->RemoveNodes(nodes_to_del.begin(), nodes_to_del.end());
    }
  }

  /**
   * @brief Set the property of node index `i`.
   *
   * @param i: the index of node.
   * @param prop: property class of the node to set.
   */
  void SetProperty(Index i, Prop prop) {
    if (!contains(i)) return;

    node_props_[i] = std::move(prop);
  }

  /**
   * @brief Set the property of edge (u, v).
   *
   * @param u: one endpoint index of edge.
   * @param v: one endpoint index of edge.
   * @param prop: property class of the edge to set.
   */
  void SetProperty(Index u, Index v, Prop prop) {
    if (!contains(u) || !contains(v)) return;

    edge_props_[std::make_tuple(u, v)] = prop;
    edge_props_[std::make_tuple(v, u)] = prop;
  }

  /**
   * @brief Get the property of node index `i`.
   *
   * @param i: the index of node.
   * @return (Prop) the property of node `i`.
   * @throw (std::out_of_range) if node does not exist.
   */
  Prop GetProperty(Index i) const { return node_props_.at(i); }

  /**
   * @brief Get the property of edge (u v).
   *
   * @param u: startpoint index of edge.
   * @param v: endpoint index of edge.
   * @return (Prop) the property of edge (u, v).
   * @throw (std::out_of_range) if edge does not exist.
   */
  Prop GetProperty(Index u, Index v) const {
    return edge_props_.at(std::make_tuple(u, v));
  }

  /**
   * @brief The total number of vertices in the graph.
   *
   * @sa Graph::NumEdges
   * @return (size_t) the number of vertices.
   */
  size_t NumNodes() const { return uniq_.size(); }

  /**
   * @brief The total number of edges in the graph.
   *
   * @sa Graph::NumNodes
   * @return (size_t) the number of edges.
   */
  size_t NumEdges() const {
    auto double_edges = std::accumulate(
        adj_.begin(), adj_.end(), 0LLU,
        [](size_t val, typename decltype(adj_)::value_type pair) {
          return val + pair.second.size();
        }
    );
    return double_edges >> 1;
  }

  /**
   * @brief Get the adjacent list of node index `i`.
   *
   * Adjacent list represents all neighboring nodes to node `i`, which also
   * means all nodes have edge with `i`.
   *
   * @param i: index of node.
   * @return (std::deque<Index>) a list of node indices adjacent to `i`.
   */
  std::deque<Index> Adjacent(Index i) const {
    if (!contains(i)) return {};
    return adj_.at(i);
  }

  /**
   * @brief Make a shallow copy of the graph, node instance is not copied.
   *
   * This function copies just node indices and edges of the graph, it's used
   * for graph algorithms, uses shall never use the shallow copied graph.
   * @return (Graph) a copy of the graph.
   */
  Graph ShallowCopy() const { return copyHelper<Graph>(); }

  /**
   * @brief Generate a shallow copy of subgraph of nodes in range(beg, end)
   *
   * Subgraph only contains nodes and edges, no hierarchical data.
   *
   * @tparam It: iterator of node index.
   * @param beg: begin of the index iterator.
   * @param end: end of the index iterator.
   * @return (Graph) a copy of the subgraph.
   */
  template <class It>
  Graph Subgraph(It beg, It end) const {
    return subgraphHelper<Graph>(beg, end);
  }

  /**
   * @brief Generate a shallow copy of subgraph of nodes in container.
   *
   * Subgraph only contains nodes and edges, no hierarchical data.
   *
   * @tparam Cont: type of the STL-like containers.
   * @param cont: the node index container.
   * @return (Graph) a copy of the subgraph.
   */
  template <class Cont>
  Graph Subgraph(const Cont &cont) const {
    return subgraphHelper<Graph>(std::begin(cont), std::end(cont));
  }

  /**
   * @brief Increase the hierarchical level of the graph and all subgraphs by 1.
   */
  void IncreaseHierarchy() {
    hierarchy_++;
    for (auto &&[_, subg] : subgraphs_) {
      subg->IncreaseHierarchy();
    }
  }

  /**
   * @brief Prepending a `prefix` to the graph name and all subgraph names.
   *
   * @param prefix: a string to prepend
   */
  void PrefixIndex(std::string_view prefix) {
    name_ = fmt::format("{}/{}", prefix, name_);
    for (auto &&[_, subg] : subgraphs_) {
      subg->PrefixIndex(prefix);
    }
  }

  /**
   * @brief Print the hierarchical level of the graph and subgraphs.
   */
  std::string PrintHierarchy(int indent = 0) {
    std::string msg =
        fmt::format("{: ^{}}({:d}) {:s} {{\n", "", indent, hierarchy_, name_);
    for (auto &&[_, subg] : subgraphs_) {
      msg += subg->PrintHierarchy(indent + 2);
    }
    msg += fmt::format("{: ^{}}{:s}", "", indent, "}\n");
    return msg;
  }

  // STL style apis
  // NOLINTBEGIN(readability-identifier-naming)

  /// @brief check whether node index `i` is in the graph.
  // NOLINTNEXTLINE(readability-identifier-naming)
  bool contains(Index i) const { return uniq_.count(i) > 0; }

  /// @brief check whether edge `u-v` is in the graph.
  // NOLINTNEXTLINE(readability-identifier-naming)
  bool contains(Index u, Index v) const {
    if (contains(u) && contains(v)) {
      auto &adj_u = adj_.at(u);
      return std::find(adj_u.begin(), adj_u.end(), v) != adj_u.end();
    }
    return false;
  }

  /// @brief check whether the graph is empty (no nodes).
  // NOLINTNEXTLINE(readability-identifier-naming)
  bool empty() const { return uniq_.empty(); }

  /// @brief clear nodes and edges and all subgraphs in the graph
  // NOLINTNEXTLINE(readability-identifier-naming)
  void clear() noexcept {
    for (auto &&[_, subg] : subgraphs_) {
      subg->clear();
    }
    subgraphs_.clear();
    adj_.clear();
    pred_.clear();
    uniq_.clear();
    node_props_.clear();
    edge_props_.clear();
    nodes_.clear();
  }

  /// @brief get the index of the graph.
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::string str() const { return name_; }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::iterator begin() { return uniq_.begin(); }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::const_iterator begin() const {
    return uniq_.begin();
  }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::const_iterator cbegin() const {
    return uniq_.cbegin();
  }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::iterator end() { return uniq_.end(); }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::const_iterator end() const {
    return uniq_.end();
  }

  /// @brief iterator for node index
  // NOLINTNEXTLINE(readability-identifier-naming)
  typename std::unordered_set<Index>::const_iterator cend() const {
    return uniq_.cend();
  }

  // NOLINTEND(readability-identifier-naming)

 protected:
  /// @brief The hierarchical level of graph, top level is 0.
  uint32_t hierarchy_ = 0;

  /// @brief A graph name used to index the graph.
  std::string name_;

  /// @brief Adjacent list of the graph.
  /// Where adj[n] is a list of nodes who has an edge with `n`.
  /// In directed graph, adj[n] is a list of nodes who has an edge **from** `n`.
  std::unordered_map<Index, std::deque<Index>> adj_;

  /// @brief A predecessor adjacent list of the digraph.
  /// Where pred_[n] is a list of nodes who has an edge **to** `n`.
  std::unordered_map<Index, std::deque<Index>> pred_;

  /// @brief A unique list to ensure any node instance in this graph is unique.
  std::unordered_set<Index> uniq_;

  /// @brief A mapping container for the properties of each node, key is the
  /// node index.
  std::unordered_map<Index, Prop> node_props_;

  /// @brief A mapping container for edge properties. P[(i, j)] is the property
  /// of edge (i, j). For non-directed graph, there's also a P[(j,i)]=P[(i,j)].
  std::unordered_map<std::tuple<Index, Index>, Prop> edge_props_;

  /// @brief A container for nodes instance. It also stores the last reference
  /// to node instance.
  std::deque<TNode> nodes_;

  /// @brief A container for nested subgraph, the node instances in subgraph do
  /// not added into `nodes_`, they remain in the `nodes_` of subgraphs. But
  /// the node index will be expanded to `adj_` and `uniq_`.
  std::unordered_map<GIndex, std::unique_ptr<Graph>> subgraphs_;

  template <class U, std::enable_if_t<std::is_base_of_v<Graph, U>, int> = 0>
  U copyHelper() const {
    U h;
    h.hierarchy_ = hierarchy_;
    h.name_ = name_;
    h.adj_.merge(CopyConstruct(adj_));
    h.pred_.merge(CopyConstruct(pred_));
    h.uniq_.merge(CopyConstruct(uniq_));
    h.node_props_.merge(CopyConstruct(node_props_));
    h.edge_props_.merge(CopyConstruct(edge_props_));
    for (auto &&[k, v] : subgraphs_) {
      auto vv = v->ShallowCopy();  // a clone of v adjacent list.
      h.subgraphs_.emplace(
          std::make_pair(k, std::make_unique<decltype(vv)>(std::move(vv)))
      );
    }
    return h;
  }

  template <
      class U,
      class It,
      std::enable_if_t<std::is_base_of_v<Graph, U>, int> = 0>
  U subgraphHelper(It beg, It end) const {
    static_assert(
        std::is_same_v<typename std::iterator_traits<It>::value_type, Index>,
        "Iterator value type is not Index"
    );

    U subg;
    subg.name_ = name_;
    for (auto it = beg; it != end; ++it) {
      if (!contains(*it)) continue;
      subg.uniq_.insert(*it);
      subg.node_props_.insert(std::make_pair(*it, node_props_.at(*it)));
    }
    for (auto it = beg; it != end; ++it) {
      if (!contains(*it)) continue;
      subg.adj_[*it] = {};
      subg.pred_[*it] = {};
      for (auto &nbr : adj_.at(*it)) {
        if (subg.uniq_.count(nbr)) {
          subg.adj_[*it].push_back(nbr);
          auto e = std::make_tuple(*it, nbr);
          subg.edge_props_[e] = edge_props_.at(e);
        }
      }
      for (auto &nbr : pred_.at(*it)) {
        if (subg.uniq_.count(nbr)) {
          subg.pred_[*it].push_back(nbr);
        }
      }
    }
    return subg;
  }
};
}  // namespace vila
#endif  // VILA_GRAPH_GRAPH_H_
