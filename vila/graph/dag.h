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
 * Description: algorithms for directed acyclic graph
 ****************************************/
#ifndef VILA_GRAPH_DAG_H_
#define VILA_GRAPH_DAG_H_
#include <deque>
#include <functional>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "vila/graph/digraph.h"
#include "vila/graph/traversal.h"

namespace vila {
/**
 * @brief Returns a list of nodes in topologically sorted order.
 *   A topological sort is a nonunique permutation of the nodes of a
 *   directed graph such that an edge from u to v implies that u
 *   appears before v in the topological sort order. This ordering is
 *   valid only if the graph has no directed cycles.
 *
 * See https://en.wikipedia.org/wiki/Topological_sorting
 *
 * @param g: a directed graph to sort
 * @param error: optional, if graph is cyclic, set error to true, otherwise set
 *   to false.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in sorted order.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> TopologicalSort(
    const DiGraph<T, Prop, TNode, NodeIndex> &g, bool *error = nullptr
) {
  using Index = typename NodeIndex::index_type;
  // count for each node, this algo performs follow steps recursively:
  // 1. find all the zero indegree nodes, put them into order list
  // 2. remove the nodes in step 1 from the graph
  // 3. go back to step 1 until all nodes in the graph are visited
  // If all nodes are visited and in the order list, the list is in topological
  // order.
  // If not all nodes are in the order list, then there must be at least one
  // cycle.
  std::unordered_map<Index, size_t> indegree_map;
  std::deque<Index> zero_indegrees;
  std::deque<Index> orders;
  for (const auto &n : g) {
    indegree_map[n] = g.InDegree(n);
    if (g.InDegree(n) == 0) {
      zero_indegrees.push_back(n);
    }
  }
  while (!zero_indegrees.empty()) {
    auto q = zero_indegrees.front();
    zero_indegrees.pop_front();
    orders.push_back(q);
    for (const auto &child : g.Successor(q)) {
      indegree_map[child]--;
      if (indegree_map[child] == 0) {
        zero_indegrees.push_back(child);
      }
    }
  }
  if (error) *error = orders.size() != g.NumNodes();
  return orders;
}

/**
 * @brief Returns a list of **all** topological sorts of DiGraph g.
 *
 * @note Reference to Knuth's algorithm[1] and implementation of
 * [networkx](https://github.com/networkx/networkx).
 * [1] Knuth, Donald E., Szwarcfiter, Jayme L. (1974).
       "A Structured Program to Generate All Topological Sorting Arrangements"
       Information Processing Letters, Volume 2, Issue 6, 1974, Pages 153-157,
       ISSN 0020-0190,
       https://doi.org/10.1016/0020-0190(74)90001-5.
       Elsevier (North-Holland), Amsterdam
 * @param g: a directed graph to sort.
 * @param error: optional, if graph is cyclic, set error to true, otherwise set
 *   to false.
 * @return (std::deque<std::deque<typename NodeIndex::index_type>>) a list of
 *   sorted orders.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto AllTopologicalSorts(  // NOLINT(readability-function-cognitive-complexity)
    const DiGraph<T, Prop, TNode, NodeIndex> &g,
    bool *error = nullptr
) -> std::deque<std::deque<typename NodeIndex::index_type>> {
  using Index = typename NodeIndex::index_type;
  std::unordered_map<Index, int64_t> count;  // count for indegrees of an index
  // NOLINTNEXTLINE(readability-identifier-*) use the name in paper
  std::deque<Index> D;  // nodes with zero indegree
  for (const auto &i : g) {
    count[i] = g.InDegree(i);
    if (count[i] == 0) D.push_back(i);
  }

  std::deque<Index> current_sort, bases;
  std::deque<decltype(bases)> all_sorts;
  do {
    if (current_sort.size() == g.NumNodes()) {
      // one sort ready
      all_sorts.push_back(current_sort);

      while (!current_sort.empty()) {
        auto q = current_sort.back();
        current_sort.pop_back();
        // restore the last states
        for (const auto &j : g.Successor(q)) {
          count[j]++;
        }
        while (!D.empty() && count[D.back()] > 0) {
          D.pop_back();
        }
        D.push_front(q);
        if (D.back() == bases.back()) {
          bases.pop_back();
        } else {
          break;
        }
      }
    } else {
      if (D.empty()) {
        // oops! graph contains cycles
        if (error) *error = true;
        return all_sorts;
      }
      // sorting, get one node of zero indegree
      auto q = D.back();
      D.pop_back();
      for (const auto &j : g.Successor(q)) {
        if (--count[j] == 0) {
          D.push_back(j);
        }
      }
      current_sort.push_back(q);
      if (bases.size() < current_sort.size()) {
        bases.push_back(q);
      }
    }
  } while (!bases.empty());
  return all_sorts;
}

namespace internal {
template <class Index, class Prop, class Less = std::less<Prop>>
struct KeyCompareHelper {
  using Pair = std::tuple<Index, Prop>;

  bool operator()(Pair a, Pair b) const {
    return Less()(std::get<1>(a), std::get<1>(b));
  }
};
}  // namespace internal

/**
 * @brief Returns a list of nodes in topologically sorted order.
 *   Sorting is based on node weights, meaning with multiple arrangements of
 *   zero indegree nodes, sorts the nodes with same weight first. Node weights
 *   are implemented using template class `Prop` which has an `std::less`
 *   implementation.
 *
 * @example
 * 0(red), 1(red), 2(blue), 3(blue), 4(blue)
 * where red < blue and edges 2->3, 3->0, 2->1, 1->4
 *     -> 3(blue) -> 0(red)
 *   /
 * 2(blue)
 *   \
 *     -> 1(red) -> 4(blue)
 * Normal sort may be [2, 3, 0, 1, 4], [2, 1, 3, 0, 4]... where the latter one
 * property is [blue, red, blue, red, blue].
 * While this sort must be [2, 3, 0, 1, 4] or [2, 0, 1, 0, 4], where the color
 * is [blue, blue, red, red, blue].
 * The same properties are always get clustered.
 *
 * @param g: a directed graph to sort
 * @param error: optional, if graph is cyclic, set error to true, otherwise set
 *   to false.
 * @return (std::deque<typename NodeIndex::index_type>) a list of node indices
 *   in sorted order.
 */
template <
    class T,
    class Prop,
    class Less = std::less<Prop>,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<typename NodeIndex::index_type> WeightedTopologicalSort(
    const DiGraph<T, Prop, TNode, NodeIndex> &g, bool *error = nullptr
) {
  using Index = typename NodeIndex::index_type;
  using Compare = internal::KeyCompareHelper<Index, Prop, Less>;
  std::unordered_map<Index, size_t> indegree_map;
  std::multiset<typename Compare::Pair, Compare> zero_indegrees;
  std::deque<Index> orders;
  // a != b
  auto ne = [&](Prop a, Prop b) -> bool {
    return Less()(a, b) || Less()(b, a);
  };

  for (const auto &n : g) {
    indegree_map[n] = g.InDegree(n);
    if (g.InDegree(n) == 0) {
      zero_indegrees.insert(std::make_tuple(n, g.GetProperty(n)));
    }
  }
  Prop beacon;
  if (!zero_indegrees.empty()) {
    beacon = std::get<1>(*zero_indegrees.cbegin());
  }
  std::stack<typename Compare::Pair> stack;
  while (!zero_indegrees.empty()) {
    auto [q, prop] = *zero_indegrees.begin();
    zero_indegrees.erase(zero_indegrees.begin());
    while (ne(beacon, prop) && !zero_indegrees.empty()) {
      // if the 1st node in zero indegrees has different property than beacon
      // stack until find one has the same property as beacon
      stack.push(std::make_tuple(q, prop));
      auto [q2, prop2] = *zero_indegrees.begin();
      zero_indegrees.erase(zero_indegrees.begin());
      q = q2;
      prop = prop2;
    }
    if (ne(beacon, prop)) {
      // there is no node has same property, change the beacon and clear the
      // stack
      beacon = prop;
      while (!stack.empty()) {
        zero_indegrees.insert(stack.top());
        stack.pop();
      }
    } else if (zero_indegrees.empty() && !stack.empty()) {
      // The last node has the same prop as beacon, must clear the stack
      // before exiting the loop.
      // Update the beacon and move nodes in stack to zero_indegrees map.
      while (!stack.empty()) {
        zero_indegrees.insert(stack.top());
        stack.pop();
      }
      beacon = std::get<1>(*zero_indegrees.cbegin());
    }
    orders.push_back(q);
    for (const auto &child : g.Successor(q)) {
      indegree_map[child]--;
      if (indegree_map[child] == 0) {
        zero_indegrees.insert(std::make_tuple(child, g.GetProperty(child)));
      }
    }
  }
  if (error) *error = orders.size() != g.NumNodes();
  return orders;
}

/**
 * @brief Generate nodes in strongly connected components of graph.
 *
 * @note In the mathematical theory of directed graphs, a graph is said to be
 * strongly connected if every vertex is reachable from every other vertex. The
 * strongly connected components of an arbitrary directed graph form a partition
 * into subgraphs that are themselves strongly connected. See
 * https://en.wikipedia.org/wiki/Strongly_connected_component
 * @note use algorithm by S. Kosaraju, see
 * https://en.wikipedia.org/wiki/Kosaraju's_algorithm
 * @sa DfsPreorder, DiGraph
 *
 * @param g: a directed graph to apply
 * @return (std::deque<std::unordered_set<typename NodeIndex::index_type>>) a
 * list of sets, where each set is a component of strongly connected node
 * indices. The returned components are in order.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
std::deque<std::unordered_set<typename NodeIndex::index_type>>
KosarajuStronglyConnectedComponents(const DiGraph<T, Prop, TNode, NodeIndex> &g
) {
  using Index = typename NodeIndex::index_type;
  // Get a post-ordering node from reversed graph
  auto h = g.ShallowCopy();
  h.Reverse();
  auto post_order = DfsPostorder(h);
  std::unordered_set<Index> visitied;
  std::deque<std::unordered_set<Index>> sccs;
  while (!post_order.empty()) {
    auto post = post_order.back();
    post_order.pop_back();
    if (visitied.count(post)) continue;
    auto pre_order = DfsPreorder(g, {post});
    std::unordered_set<Index> scc;
    for (const auto &pre : pre_order) {
      if (!visitied.count(pre)) {
        scc.insert(pre);
        visitied.insert(pre);
      }
    }
    sccs.push_back(scc);
  }
  return sccs;
}

template <class T>
double LongestPathDefaultWeight(const T & /*u*/, const T & /*v*/) {
  return 1.0;
}

/**
 * @brief Get the longest path in a DAG `g`.
 *
 * @note referenced from [networkx](https://github.com/networkx/networkx)
 *
 * @tparam Fn: A function type that can compute weight of edge (u,v).
 * @param g: a DAG graph.
 * @param weight: A function pointer that can compute weight of edge (u,v).
 * @return (std::deque<typename NodeIndex::index_type>) a list of indices for
 * longest path in the graph.
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>,
    class Fn = std::function<double(
        const typename NodeIndex::index_type &u,
        const typename NodeIndex::index_type &v
    )>>
std::deque<typename NodeIndex::index_type>
DagLongestPath(const DiGraph<T, Prop, TNode, NodeIndex> &g, Fn weight = LongestPathDefaultWeight<typename NodeIndex::index_type>) {
  using Index = typename NodeIndex::index_type;
  bool error = false;
  auto topo_order = TopologicalSort(g, &error);
  if (error || g.NumNodes() == 0) return {};  // not a DAG or empty

  // mapping is {v: (distance, u)}
  // the longest distance from v to u
  std::unordered_map<Index, std::tuple<double, Index>> dist;
  std::tuple<double, Index> max_u{};
  Index x{};
  for (const auto &u : topo_order) {
    std::tuple<double, Index> max_v{0, u};
    for (const auto &v : g.Predecessor(u)) {
      auto [v_to_u, _] = dist[v];
      v_to_u += weight(v, u);
      if (v_to_u >= std::get<0>(max_v)) {
        std::get<0>(max_v) = v_to_u;
        std::get<1>(max_v) = v;
      }
    }
    if (std::get<0>(max_v) >= 0) {
      dist[u] = max_v;
    } else {
      dist[u] = {0, u};
    }
    if (std::get<0>(dist[u]) > std::get<0>(max_u)) {
      max_u = dist[u];
      x = u;
    }
  }

  std::deque<Index> path;
  Index y{};
  while (x != y) {
    path.push_back(x);
    y = x;
    x = std::get<1>(dist[x]);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

/**
 * @brief Find simple cycles (elementary circuits) of a directed graph.
 *  A `simple cycle`, or `elementary circuit`, is a closed path where
 *  no node appears twice. Two elementary circuits are distinct if they
 *  are not cyclic permutations of each other.
 *
 * @note this function implements Johnson's algorithm [1].
 * @note referenced from [networkx](https://github.com/networkx/networkx)
 *
 * reference
 * [1] Finding all the elementary circuits of a directed graph.
 *     D. B. Johnson, SIAM Journal on Computing 4, no. 1, 77-84, 1975.
 *     https://doi.org/10.1137/0204007
 *
 * @param g
 * @return (std::deque<std::unordered_set<typename NodeIndex::index_type>>)
 */
template <
    class T,
    class Prop = DummyProperty,
    class TNode = Node<T>,
    class NodeIndex = AddressIndex<T>>
auto SimpleCycles(  // NOLINT(readability-function-cognitive-complexity)
    const DiGraph<T, Prop, TNode, NodeIndex> &g
) -> std::deque<std::unordered_set<typename NodeIndex::index_type>> {
  using Index = typename NodeIndex::index_type;
  using Cycle = std::unordered_set<Index>;
  using Path = std::deque<Index>;
  std::deque<Cycle> cycles;

  auto unblock_fn = [](Index thisnode, Cycle &blocked,
                       std::unordered_map<Index, Cycle> &bmap) {
    Cycle stack{thisnode};
    while (!stack.empty()) {
      auto node = *stack.begin();
      stack.erase(stack.begin());
      if (blocked.count(node) > 0) {
        blocked.erase(node);
        stack.merge(bmap[node]);
        bmap[node].clear();
      }
    }
  };

  auto h = g.ShallowCopy();  // make a shallow copy from g
  auto sccs = KosarajuStronglyConnectedComponents(h);
  // keep scc with more than 2 nodes
  sccs.erase(
      std::remove_if(
          sccs.begin(), sccs.end(), [](const Cycle &c) { return c.size() == 1; }
      ),
      sccs.end()
  );
  // Johnson's algorithm exclude self cycle edges like (v, v). To be backward
  // compatible, we record those cycles in advance and then remove from h
  for (const auto &n : h) {
    if (h.contains(n, n)) {
      cycles.push_back({n});
      h.RemoveEdge(n, n);
    }
  }
  while (!sccs.empty()) {
    Path path;
    Cycle blocked;                       // vertex: blocked from search?
    Cycle closed;                        // nodes involved in a cycle
    std::unordered_map<Index, Cycle> B;  // NOLINT(readability-*) name in paper

    auto scc = sccs.back();
    sccs.pop_back();
    auto scc_h = h.Subgraph(scc);
    auto startnode = *scc.begin();
    scc.erase(scc.begin());
    path.push_back(startnode);
    blocked.insert(startnode);
    // a stack in dfs, (index, neighbors)
    std::stack<std::tuple<Index, std::deque<Index>>> stack;
    stack.push(std::make_tuple(startnode, scc_h.Successor(startnode)));
    while (!stack.empty()) {
      auto &&[thisnode, nbrs] = stack.top();
      if (nbrs.empty()) {
        if (closed.count(thisnode)) {
          unblock_fn(thisnode, blocked, B);
        } else {
          for (auto &nbr : scc_h.Successor(thisnode)) {
            if (B[nbr].count(thisnode) == 0) {
              B[nbr].insert(thisnode);
            }
          }
        }
        stack.pop();
        path.pop_back();
      } else {
        auto nextnode = nbrs.back();
        nbrs.pop_back();
        if (nextnode == startnode) {
          // found a cycle
          cycles.push_back(Cycle(path.begin(), path.end()));
          closed.merge(Cycle(path.begin(), path.end()));
        } else if (blocked.count(nextnode) == 0) {
          // node not visited
          path.push_back(nextnode);
          blocked.insert(nextnode);
          closed.erase(nextnode);
          stack.push(std::make_tuple(nextnode, scc_h.Successor(nextnode)));
          continue;
        }
      }
    }
    // done processing this node
    if (scc.size() <= 1) continue;
    scc_h = h.Subgraph(scc);
    for (const auto &smaller_scc : KosarajuStronglyConnectedComponents(scc_h)) {
      if (smaller_scc.size() > 1) sccs.push_back(smaller_scc);
    }
  }

  return cycles;
}
}  // namespace vila
#endif  // VILA_GRAPH_DAG_H_
