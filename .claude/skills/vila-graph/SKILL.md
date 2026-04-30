---
name: vila-graph
description: Guidance for using the vila/graph component - template header-only DAG (Directed Acyclic Graph) library. Use this skill when working with graph data structures, directed graphs, topological sorting, DFS/BFS traversal, cycle detection, path finding, or graph algorithms in C++.
---

# VILA Graph - Template DAG Library

Header-only graph library with directed/undirected graphs and algorithms.

## Headers

```cpp
#include "vila/graph/graph.h"      // Undirected graph
#include "vila/graph/digraph.h"    // Directed graph
#include "vila/graph/dag.h"        // DAG algorithms
#include "vila/graph/node.h"       // Node container
#include "vila/graph/traversal.h"  // DFS/BFS traversal
#include "vila/graph/route.h"      // Path finding
```

## Bazel Target

```bazel
deps = [
    "//vila/graph",
    "//vila/graph:dag",
    "//vila/graph:route",
    "//vila/graph:traversal",
]
```

## Graph Types

### Node Container

```cpp
#include "vila/graph/node.h"

// Node holds unique_ptr to instance T
vila::Node<int> node(new int(42));
vila::Node<int> node(std::make_unique<int>(42));

node.get();       // returns int* (raw pointer)
node.operator->(); // STL-style access
```

### Graph (Undirected)

```cpp
#include "vila/graph/graph.h"

vila::Graph<int> g("main");  // named graph

// Add nodes (must move ownership)
g.AddNode(vila::Node<int>(new int(1)));
g.AddNode(vila::Node<int>(new int(2)));

// Add nodes from container
std::vector<vila::Node<int>> nodes = {...};
g.AddNodes(std::move(nodes));

// Add edges (between node pointers)
int* n1 = g.GetNodes().begin();  // get index
int* n2 = ...;
g.AddEdge(n1, n2);

// Query
g.NumNodes();
g.NumEdges();
g.contains(node_ptr);            // check if node exists
g.contains(u, v);                // check if edge exists
g.Adjacent(node_ptr);            // get neighbors

// Properties (optional)
g.SetProperty(node_ptr, MyProp{});
g.GetProperty(node_ptr);

// Subgraphs (hierarchical)
g.AddGraph(vila::Graph<int>("subgraph"));  // nested graph
g.RemoveGraph("subgraph");
g.PrintHierarchy();  // show structure

// Create shallow copy (for algorithms)
auto copy = g.ShallowCopy();
auto subg = g.Subgraph(node_set);
```

### DiGraph (Directed)

```cpp
#include "vila/graph/digraph.h"

vila::DiGraph<int> dg("main");

dg.AddEdge(u, v);  // directed: u -> v

dg.Successor(u);   // nodes u points to
dg.Predecessor(u); // nodes pointing to u
dg.InDegree(u);    // number of incoming edges
dg.OutDegree(u);   // number of outgoing edges

dg.Reverse();      // reverse all edge directions

// Shallow copy and subgraph
auto copy = dg.ShallowCopy();
auto subg = dg.Subgraph(nodes);
```

## DAG Algorithms (dag.h)

```cpp
#include "vila/graph/dag.h"

// Topological sort
bool error = false;
auto order = vila::TopologicalSort(dg, &error);
// order is deque of node indices; error=true if cyclic

// All topological sorts (Knuth algorithm)
auto all_orders = vila::AllTopologicalSorts(dg, &error);

// Weighted topological sort (cluster by property)
auto order = vila::WeightedTopologicalSort<MyProp>(dg, &error);

// Strongly connected components (Kosaraju)
auto sccs = vila::KosarajuStronglyConnectedComponents(dg);
// returns deque of unordered_set of node indices

// Longest path in DAG
auto path = vila::DagLongestPath(dg);
auto path = vila::DagLongestPath(dg, [](auto u, auto v) { return weight; });

// Simple cycles (Johnson's algorithm)
auto cycles = vila::SimpleCycles(dg);
```

## Traversal (traversal.h)

```cpp
#include "vila/graph/traversal.h"

// DFS edges
auto edges = vila::DfsEdges(dg);                        // all nodes
auto edges = vila::DfsEdges(dg, {start_node}, depth);   // from sources

// DFS with labels
auto labeled = vila::DfsLabeledEdges(dg);
// returns deque of (u, v, edge_label) tuples
// edge_label: forward, nontree, reverse

// DFS preorder/postorder
auto preorder = vila::DfsPreorder(dg);
auto preorder = vila::DfsPreorder(dg, {start_node});
auto postorder = vila::DfsPostorder(dg);

// BFS edges (works on undirected graphs too)
auto edges = vila::BfsEdges(g);
auto edges = vila::BfsEdges(g, {start_node}, depth_limit);
```

## Path Finding (route.h)

```cpp
#include "vila/graph/route.h"

// Check if nodes form simple path
bool is_path = vila::IsSimplePath(g, node_sequence);

// All simple paths from source to target
auto paths = vila::AllSimplePaths(g, source, target);
auto paths = vila::AllSimplePaths(g, source, target, depth_limit);

// All paths from source to any leaf
auto paths = vila::AllSimplePaths(dg, source);

// All root-to-leaf paths
auto paths = vila::RootToLeafPaths(dg);
```

## Indexing

Default `AddressIndex<T>` uses raw pointers as indices:

```cpp
vila::AddressIndex<int>::index_type = int*;

// Custom indexer
struct MyIndex {
    using index_type = int;  // or any hashable type
    index_type operator()(const Node<T>& node) const;
};
```

## Common Patterns

### Pipeline/DAG Pattern

```cpp
vila::DiGraph<Task> pipeline("pipeline");
for (auto& task : tasks) {
    pipeline.AddNode(vila::Node<Task>(new Task(std::move(task))));
}
for (auto& [from, to] : dependencies) {
    pipeline.AddEdge(from, to);
}

bool cyclic = false;
auto order = vila::TopologicalSort(pipeline, &cyclic);
if (cyclic) { /* handle cycle */ }

for (auto* task_ptr : order) {
    task_ptr->execute();
}
```
