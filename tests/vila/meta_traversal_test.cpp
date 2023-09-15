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
 * Description: test traversal algorithms
 ****************************************/
#include <gtest/gtest.h>

#include "vila/graph/traversal.h"

using namespace vila;  // NOLINT(*-using-namespace)

TEST(DFS, DfsEdges) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
  }
  g.AddNodes(nodes.begin(), nodes.end());
  for (int i = 0; i < 4; i++) {
    g.AddEdge(indices[i], indices[i + 1]);
  }
  auto edges = DfsEdges(g, {indices[0]});
  std::deque<std::tuple<int *, int *>> expected({
      {indices[0], indices[1]},
      {indices[1], indices[2]},
      {indices[2], indices[3]},
      {indices[3], indices[4]}
  });
  EXPECT_EQ(edges, expected);
  edges = DfsEdges(g, {indices[0]});
  EXPECT_EQ(edges, expected);
}

TEST(DFS, DfsLabeledEdges) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
  }
  g.AddNodes(nodes.begin(), nodes.end());
  for (int i = 0; i < 4; i++) {
    g.AddEdge(indices[i], indices[i + 1]);
  }
  auto labeled_edges = DfsLabeledEdges(g, {indices[4]});
  std::deque<std::tuple<int *, int *, edge_label>> expected({
      {indices[4], indices[4], edge_label::forward},
      {indices[4], indices[4], edge_label::reverse},
  });
  EXPECT_EQ(labeled_edges, expected);
  g.AddEdge(indices[4], indices[0]);
  labeled_edges = DfsLabeledEdges(g, {indices[0]});
  expected = {
      {indices[0], indices[0], edge_label::forward},
      {indices[0], indices[1], edge_label::forward},
      {indices[1], indices[2], edge_label::forward},
      {indices[2], indices[3], edge_label::forward},
      {indices[3], indices[4], edge_label::forward},
      {indices[4], indices[0], edge_label::nontree},
      {indices[3], indices[4], edge_label::reverse},
      {indices[2], indices[3], edge_label::reverse},
      {indices[1], indices[2], edge_label::reverse},
      {indices[0], indices[1], edge_label::reverse},
      {indices[0], indices[0], edge_label::reverse},
  };
  EXPECT_EQ(labeled_edges, expected);
}

TEST(BFS, BfsEdges) {
  Graph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
  }
  g.AddNodes(nodes.begin(), nodes.end());
  for (int i = 0; i < 4; i++) {
    g.AddEdge(indices[i], indices[i + 1]);
  }
  std::deque<std::tuple<int *, int *>> expected({
      {indices[0], indices[1]},
      {indices[1], indices[2]},
      {indices[2], indices[3]},
      {indices[3], indices[4]}
  });
  auto edges = BfsEdges(g, {indices[0]});
  EXPECT_EQ(edges, expected);
}
