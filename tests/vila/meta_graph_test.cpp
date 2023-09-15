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
 * Description: test graph and digraph
 ****************************************/
#include <gtest/gtest.h>

#include <deque>

#include "vila/graph/digraph.h"
#include "vila/graph/graph.h"
#include "vila/graph/node.h"
#include "vila/graph/property.h"

using namespace vila;  // NOLINT(*-using-namespace)

TEST(NodeTest, NodeConstructRawPointer) {
  int* a = new int;
  Node<int> n(a);
  EXPECT_EQ(a, n.get());
}

TEST(NodeTest, NodeConstructUniquePtr) {
  std::unique_ptr<int> a(new int);
  Node<int> n(std::move(a));
  EXPECT_EQ(a.get(), nullptr);
  EXPECT_NE(n.get(), nullptr);
}

TEST(NodeTest, NodeAddressIndex) {
  Node<int> n(new int);
  AddressIndex<int> index;
  EXPECT_EQ(index(n), n.get());
}

TEST(GraphTest, GraphConstruct) {
  Graph<int> g("int_graph");
  EXPECT_TRUE(g.empty());
  EXPECT_EQ(g.NumNodes(), 0);
  EXPECT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, AddAndRemoveNode) {
  Graph<int> g("int_graph");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  g.AddNode(std::move(n1));
  g.AddNode(std::move(n2));
  EXPECT_EQ(g.GetNodes().size(), 2);
  EXPECT_EQ(g.GetNodeDeque().size(), 2);
  EXPECT_FALSE(g.empty());
  EXPECT_EQ(g.NumNodes(), 2);
  EXPECT_TRUE(g.contains(ins1));
  g.RemoveNode(ins1);
  EXPECT_EQ(g.GetNodes().size(), 1);
  EXPECT_EQ(g.GetNodeDeque().size(), 1);
  EXPECT_EQ(g.NumNodes(), 1);
  EXPECT_EQ(g.NumEdges(), 0);
  EXPECT_FALSE(g.contains(ins1));
  g.clear();
  EXPECT_EQ(g.NumNodes(), 0);
  EXPECT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, AddNodesFromContainerIterator) {
  std::deque<Node<int>> node_list;
  node_list.emplace_back(new int);
  node_list.emplace_back(new int);
  node_list.emplace_back(new int);
  Graph<int> g("int_graph");
  g.AddNodes(node_list.begin(), node_list.end());
  EXPECT_EQ(g.NumNodes(), node_list.size());
  EXPECT_EQ(g.NumNodes(), node_list.size());
}

TEST(GraphTest, AddNodesFromContainer) {
  std::deque<Node<int>> node_list;
  node_list.emplace_back(new int);
  node_list.emplace_back(new int);
  node_list.emplace_back(new int);
  Graph<int> g("int_graph");
  g.AddNodes(std::move(node_list));
  EXPECT_EQ(g.NumNodes(), node_list.size());
  EXPECT_EQ(g.NumNodes(), node_list.size());
}

TEST(GraphTest, AddAndRemoveEdge) {
  Graph<int> g("int_graph");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(ins1, ins2);
  // instance 1 and 2 has not been added
  EXPECT_EQ(g.NumEdges(), 0);
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.NumNodes(), 2);
  EXPECT_TRUE(g.contains(ins1, ins2));
  EXPECT_TRUE(g.contains(ins2, ins1));
  // undirected graph edge 1-2 is equivalent to 2-1
  g.RemoveEdge(ins2, ins1);
  EXPECT_EQ(g.NumEdges(), 0);
  EXPECT_EQ(g.NumNodes(), 2);
}

TEST(GraphTest, AddAndRemoveShallowGraph) {
  Graph<int> g("main");  // 1 node
  Graph<int> s("sub");   // 2 nodes, 1 edge
  Node<int> n1(new int);
  Node<int> n2(new int);
  Node<int> n3(new int);
  int* ins2 = n2.get();
  int* ins3 = n3.get();
  g.AddNode(std::move(n1));
  s.AddNode(std::move(n2));
  s.AddNode(std::move(n3));
  s.AddEdge(ins2, ins3);
  g.AddGraph(std::move(s));
  EXPECT_EQ(g.NumNodes(), 3);
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.GetNodes().size(), 3);
  EXPECT_EQ(g.GetNodeDeque().size(), 3);
  g.RemoveGraph("sub");
  EXPECT_EQ(g.NumNodes(), 1);
  EXPECT_EQ(g.NumEdges(), 0);
  EXPECT_EQ(g.GetNodes().size(), 1);
  EXPECT_EQ(g.GetNodeDeque().size(), 1);
  g.clear();
  EXPECT_EQ(g.NumNodes(), 0);
  EXPECT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, RemoveDeepGraph) {
  Graph<int> g("main");     // 1 node
  Graph<int> s("sub");      // 2 nodes, 1 edge
  Graph<int> ss("subsub");  // 3 nodes, 2 edges
  Node<int> n1(new int);
  Node<int> n2(new int);
  Node<int> n3(new int);
  Node<int> n4(new int);
  Node<int> n5(new int);
  Node<int> n6(new int);
  int* ins2 = n2.get();
  int* ins3 = n3.get();
  int* ins4 = n4.get();
  int* ins5 = n5.get();
  int* ins6 = n6.get();
  g.AddNode(std::move(n1));
  s.AddNode(std::move(n2));
  s.AddNode(std::move(n3));
  s.AddEdge(ins2, ins3);
  ss.AddNode(std::move(n4));
  ss.AddNode(std::move(n5));
  ss.AddNode(std::move(n6));
  ss.AddEdge(ins4, ins5);
  ss.AddEdge(ins5, ins6);
  s.AddGraph(std::move(ss));
  g.AddGraph(std::move(s));
  EXPECT_EQ(g.NumNodes(), 6);
  EXPECT_EQ(g.NumEdges(), 3);
  EXPECT_EQ(g.GetNodes().size(), 6);
  EXPECT_EQ(g.GetNodeDeque().size(), 6);
  // invalid index
  EXPECT_NO_THROW(g.RemoveGraph("sub/foo"));   // NOLINT(hicpp-avoid-goto)
  EXPECT_NO_THROW(g.RemoveGraph("/foo"));      // NOLINT(hicpp-avoid-goto)
  EXPECT_NO_THROW(g.RemoveGraph("main/foo"));  // NOLINT(hicpp-avoid-goto)
  EXPECT_EQ(g.NumNodes(), 6);
  EXPECT_EQ(g.NumEdges(), 3);
  g.RemoveGraph("sub/subsub");
  EXPECT_EQ(g.NumNodes(), 3);
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.GetNodes().size(), 3);
  EXPECT_EQ(g.GetNodeDeque().size(), 3);
  g.clear();
  EXPECT_EQ(g.NumNodes(), 0);
  EXPECT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, AdjacentList) {
  Graph<int> g("int_graph");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(ins1, ins2);
  // instance 1 and 2 has not been added
  EXPECT_EQ(g.NumEdges(), 0);
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.NumNodes(), 2);
  EXPECT_EQ(g.Adjacent(ins1), std::deque<int*>({ins2}));
  EXPECT_EQ(g.Adjacent(ins2), std::deque<int*>({ins1}));
  // index not existing
  EXPECT_NO_THROW(g.Adjacent(nullptr));  // NOLINT(hicpp-avoid-goto)
}

TEST(GraphTest, ShallowCopy) {
  Graph<int> g("int_graph");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.NumNodes(), 2);
  auto h = g.ShallowCopy();
  EXPECT_EQ(h.NumNodes(), g.NumNodes());
  EXPECT_EQ(h.NumEdges(), g.NumEdges());
  h.RemoveEdge(ins1, ins2);
  EXPECT_NE(h.NumEdges(), g.NumEdges());
}

TEST(GraphTest, Subgraph) {
  Graph<int> g("main");     // 1 node
  Graph<int> s("sub");      // 2 nodes, 1 edge
  Graph<int> ss("subsub");  // 3 nodes, 2 edges
  Node<int> n1(new int);
  Node<int> n2(new int);
  Node<int> n3(new int);
  Node<int> n4(new int);
  Node<int> n5(new int);
  Node<int> n6(new int);
  int* ins2 = n2.get();
  int* ins3 = n3.get();
  int* ins4 = n4.get();
  int* ins5 = n5.get();
  int* ins6 = n6.get();
  g.AddNode(std::move(n1));
  s.AddNode(std::move(n2));
  s.AddNode(std::move(n3));
  s.AddEdge(ins2, ins3);
  ss.AddNode(std::move(n4));
  ss.AddNode(std::move(n5));
  ss.AddNode(std::move(n6));
  ss.AddEdge(ins4, ins5);
  ss.AddEdge(ins5, ins6);
  s.AddGraph(std::move(ss));
  g.AddGraph(std::move(s));
  std::deque<int*> subindice{ins2, ins3, ins4, nullptr};
  auto h = g.Subgraph(subindice.begin(), subindice.end());
  EXPECT_EQ(h.NumNodes(), 3);
  EXPECT_EQ(h.NumEdges(), 1);
}

TEST(GraphTest, PrintHierarchicalLevel) {
  Graph<int> g("main");     // 1 node
  Graph<int> s("sub");      // 2 nodes, 1 edge
  Graph<int> ss("subsub");  // 3 nodes, 2 edges
  Node<int> n1(new int);
  Node<int> n2(new int);
  Node<int> n3(new int);
  Node<int> n4(new int);
  Node<int> n5(new int);
  Node<int> n6(new int);
  int* ins2 = n2.get();
  int* ins3 = n3.get();
  int* ins4 = n4.get();
  int* ins5 = n5.get();
  int* ins6 = n6.get();
  g.AddNode(std::move(n1));
  s.AddNode(std::move(n2));
  s.AddNode(std::move(n3));
  s.AddEdge(ins2, ins3);
  ss.AddNode(std::move(n4));
  ss.AddNode(std::move(n5));
  ss.AddNode(std::move(n6));
  ss.AddEdge(ins4, ins5);
  ss.AddEdge(ins5, ins6);
  s.AddGraph(std::move(ss));
  g.AddGraph(std::move(s));
  std::cout << g.PrintHierarchy();
}

TEST(GraphTest, PropertyTest) {
  struct Property {
    bool a = true;
    int b = 1;
  };

  Graph<int, Property> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddNode(std::move(n1));
  g.AddNode(std::move(n2));
  g.SetProperty(ins1, Property{false, 2});
  EXPECT_EQ(g.GetProperty(ins1).a, false);
  EXPECT_EQ(g.GetProperty(ins1).b, 2);
  EXPECT_EQ(g.GetProperty(ins2).a, true);
  EXPECT_EQ(g.GetProperty(ins2).b, 1);
}

TEST(GraphTest, EdgePropertyTest) {
  struct Property {
    bool a = true;
    int b = 1;
  };

  Graph<int, Property> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddNode(std::move(n1));
  g.AddNode(std::move(n2));
  g.AddEdge(ins1, ins2);
  g.SetProperty(ins1, ins2, Property{false, 2});
  EXPECT_EQ(g.GetProperty(ins1, ins2).a, false);
  EXPECT_EQ(g.GetProperty(ins1, ins2).b, 2);
  EXPECT_EQ(g.GetProperty(ins2, ins1).a, false);
  EXPECT_EQ(g.GetProperty(ins2, ins1).b, 2);

  g.RemoveEdge(ins2, ins1);
  // NOLINTNEXTLINE(hicpp-avoid-goto)
  EXPECT_THROW(g.GetProperty(ins1, ins2), std::out_of_range);
}

TEST(DiGraphTest, DirectedEdge) {
  DiGraph<int> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumNodes(), 2);
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_TRUE(g.contains(ins1, ins2));
  EXPECT_FALSE(g.contains(ins2, ins1));
  g.AddEdge(ins2, ins1);
  EXPECT_EQ(g.NumEdges(), 2);
  g.RemoveEdge(ins1, ins2);
  EXPECT_FALSE(g.contains(ins1, ins2));
  EXPECT_TRUE(g.contains(ins2, ins1));
  EXPECT_EQ(g.NumEdges(), 1);
}

TEST(DiGraphTest, AdjacentList) {
  DiGraph<int> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumNodes(), 2);
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.Successor(ins1), std::deque<int*>({ins2}));
  EXPECT_EQ(g.Successor(ins2), std::deque<int*>({}));
  EXPECT_EQ(g.Predecessor(ins1), std::deque<int*>({}));
  EXPECT_EQ(g.Predecessor(ins2), std::deque<int*>({ins1}));
  g.AddEdge(ins2, ins1);
  EXPECT_EQ(g.NumEdges(), 2);
  EXPECT_EQ(g.Successor(ins2), std::deque<int*>({ins1}));
  EXPECT_EQ(g.Predecessor(ins1), std::deque<int*>({ins2}));
  g.RemoveEdge(ins1, ins2);
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.Successor(ins2), std::deque<int*>({ins1}));
  EXPECT_EQ(g.Predecessor(ins1), std::deque<int*>({ins2}));
}

TEST(DiGraphTest, Degrees) {
  DiGraph<int> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.InDegree(ins1), 0);
  EXPECT_EQ(g.InDegree(ins2), 1);
  EXPECT_EQ(g.OutDegree(ins1), 1);
  EXPECT_EQ(g.OutDegree(ins2), 0);
  // index does not exist.
  EXPECT_EQ(g.InDegree(nullptr), -1);
  EXPECT_EQ(g.OutDegree(nullptr), -1);
}

TEST(DiGraphTest, Reverse) {
  DiGraph<int> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_TRUE(g.contains(ins1, ins2));
  EXPECT_FALSE(g.contains(ins2, ins1));
  g.Reverse();
  EXPECT_TRUE(g.contains(ins2, ins1));
  EXPECT_FALSE(g.contains(ins1, ins2));
}

TEST(DiGraphTest, ShallowCopy) {
  DiGraph<int> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddEdge(std::move(n1), std::move(n2));
  EXPECT_EQ(g.NumEdges(), 1);
  EXPECT_EQ(g.NumNodes(), 2);
  auto h = g.ShallowCopy();
  EXPECT_EQ(h.NumNodes(), g.NumNodes());
  EXPECT_EQ(h.NumEdges(), g.NumEdges());
  h.RemoveEdge(ins1, ins2);
  EXPECT_NE(h.NumEdges(), g.NumEdges());
}

TEST(DiGraphTest, Subgraph) {
  DiGraph<int> g("main");     // 1 node
  DiGraph<int> s("sub");      // 2 nodes, 1 edge
  DiGraph<int> ss("subsub");  // 3 nodes, 2 edges
  Node<int> n1(new int);
  Node<int> n2(new int);
  Node<int> n3(new int);
  Node<int> n4(new int);
  Node<int> n5(new int);
  Node<int> n6(new int);
  int* ins2 = n2.get();
  int* ins3 = n3.get();
  int* ins4 = n4.get();
  int* ins5 = n5.get();
  int* ins6 = n6.get();
  g.AddNode(std::move(n1));
  s.AddNode(std::move(n2));
  s.AddNode(std::move(n3));
  s.AddEdge(ins2, ins3);
  ss.AddNode(std::move(n4));
  ss.AddNode(std::move(n5));
  ss.AddNode(std::move(n6));
  ss.AddEdge(ins4, ins5);
  ss.AddEdge(ins5, ins6);
  s.AddGraph(std::move(ss));
  g.AddGraph(std::move(s));
  std::deque<int*> subindice{ins2, ins3, ins4, nullptr};
  auto h = g.Subgraph(subindice);
  EXPECT_EQ(h.NumNodes(), 3);
  EXPECT_EQ(h.NumEdges(), 1);
}

TEST(DiGraphTest, PropertyTest) {
  struct Property {
    bool a = true;
    int b = 1;
  };

  DiGraph<int, Property> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddNode(std::move(n1));
  g.AddNode(std::move(n2));
  g.SetProperty(ins1, Property{false, 2});
  EXPECT_EQ(g.GetProperty(ins1).a, false);
  EXPECT_EQ(g.GetProperty(ins1).b, 2);
  EXPECT_EQ(g.GetProperty(ins2).a, true);
  EXPECT_EQ(g.GetProperty(ins2).b, 1);
}

TEST(DiGraphTest, EdgePropertyTest) {
  struct Property {
    bool a = true;
    int b = 1;
  };

  DiGraph<int, Property> g("main");
  Node<int> n1(new int);
  Node<int> n2(new int);
  int* ins1 = n1.get();
  int* ins2 = n2.get();
  g.AddNode(std::move(n1));
  g.AddNode(std::move(n2));
  g.AddEdge(ins1, ins2);
  g.SetProperty(ins1, ins2, Property{false, 2});
  EXPECT_EQ(g.GetProperty(ins1, ins2).a, false);
  EXPECT_EQ(g.GetProperty(ins1, ins2).b, 2);

  // NOLINTNEXTLINE(hicpp-avoid-goto)
  EXPECT_THROW(g.GetProperty(ins2, ins1), std::out_of_range);
}

TEST(PropertyTest, PropertyFormat) {
  fmt::print("{}\n", node::color::black);
  fmt::print("{}\n", node::shape::circle);
  fmt::print("{}\n", node::style::dashed);
  fmt::print("{}\n", node::style::dashed | node::style::diagonals);
}
