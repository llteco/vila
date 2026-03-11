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
