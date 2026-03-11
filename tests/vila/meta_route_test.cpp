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
 * Description: test route algorithms
 ****************************************/
#include <gtest/gtest.h>

#include "vila/graph/route.h"

using namespace vila;  // NOLINT(*-using-namespace)

TEST(PathTest, IsSimplePathGraph) {
  Graph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[0]);
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[1], ind[3]);
  g.AddEdge(ind[4], ind[0]);
  EXPECT_FALSE(IsSimplePath(g, {}));
  EXPECT_TRUE(IsSimplePath(g, {ind[0]}));
  EXPECT_TRUE(IsSimplePath(g, {ind[0], ind[1], ind[2]}));
  EXPECT_FALSE(IsSimplePath(g, {ind[0], ind[1], ind[2], ind[3]}));
  EXPECT_FALSE(IsSimplePath(g, {ind[0], ind[0]}));
}

TEST(PathTest, IsSimplePathDiGraph) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[0]);
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[1], ind[3]);
  g.AddEdge(ind[4], ind[0]);
  EXPECT_FALSE(IsSimplePath(g, {}));
  EXPECT_TRUE(IsSimplePath(g, {ind[0]}));
  EXPECT_TRUE(IsSimplePath(g, {ind[0], ind[1], ind[2]}));
  EXPECT_FALSE(IsSimplePath(g, {ind[0], ind[1], ind[2], ind[3]}));
  EXPECT_FALSE(IsSimplePath(g, {ind[0], ind[0]}));
}

TEST(PathTest, FindSimplePaths) {
  Graph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  auto paths = AllSimplePaths(g, ind[0], ind[3]);
  EXPECT_EQ(paths.size(), 1);
  EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[1], ind[2], ind[3]}));
}

TEST(PathTest, DiGraphRootToLeafPaths) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[0], ind[4]);
  auto paths = RootToLeafPaths(g);
  // sort by path length
  std::sort(
      paths.begin(), paths.end(),
      [](const std::deque<int*>& p, const std::deque<int*>& q) {
        return p.size() < q.size();
      }
  );
  EXPECT_EQ(paths.size(), 2);
  EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[4]}));
  EXPECT_EQ(paths[1], std::deque<int*>({ind[0], ind[1], ind[2], ind[3]}));
}

TEST(PathTest, FindSimplePathsWithNoTargetSpecified) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[2], ind[4]);
  auto paths = AllSimplePaths(g, ind[0]);
  EXPECT_EQ(paths.size(), 2);
  // paths are out of orders
  if (paths[0] == std::deque<int*>({ind[0], ind[1], ind[2], ind[4]})) {
    EXPECT_EQ(paths[1], std::deque<int*>({ind[0], ind[1], ind[2], ind[3]}));
  } else {
    EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[1], ind[2], ind[3]}));
    EXPECT_EQ(paths[1], std::deque<int*>({ind[0], ind[1], ind[2], ind[4]}));
  }
}

TEST(PathTest, FindSimplePathsWithDepthLimit) {
  Graph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[2], ind[4]);
  auto paths = AllSimplePaths(g, ind[0], ind[4], 3);
  EXPECT_EQ(paths.size(), 1);
  EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[1], ind[2], ind[4]}));
  paths = AllSimplePaths(g, ind[0], ind[4], 2);
  EXPECT_EQ(paths.size(), 0);
}

TEST(PathTest, FindSimplePathsWithCycle) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[0]);
  g.AddEdge(ind[1], ind[3]);
  // ignore cycles
  auto paths = AllSimplePaths(g, ind[0], ind[3]);
  EXPECT_EQ(paths.size(), 1);
  EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[1], ind[3]}));
  // in cycles
  paths = AllSimplePaths(g, ind[0], ind[2]);
  EXPECT_EQ(paths.size(), 1);
  EXPECT_EQ(paths[0], std::deque<int*>({ind[0], ind[1], ind[2]}));
}

TEST(PathTest, FindSimplePathsSameSourceTarget) {
  Graph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[0]);
  g.AddEdge(ind[1], ind[3]);
  auto paths = AllSimplePaths(g, ind[0], ind[0]);
  EXPECT_EQ(paths.size(), 0);
}

TEST(PathTest, FindSimplePathsNonTrivialGraph) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int*> ind;
  for (int i = 0; i < 6; i++) {
    nodes.emplace_back(new int);
    ind.push_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[3], ind[4]);
  g.AddEdge(ind[4], ind[5]);
  g.AddEdge(ind[0], ind[5]);
  g.AddEdge(ind[1], ind[5]);
  g.AddEdge(ind[1], ind[3]);
  g.AddEdge(ind[5], ind[4]);
  g.AddEdge(ind[4], ind[2]);
  g.AddEdge(ind[4], ind[3]);
  auto paths = AllSimplePaths(g, ind[1], ind[3]);
  EXPECT_EQ(paths.size(), 4);
  std::sort(
      paths.begin(), paths.end(),
      [](const std::deque<int*>& p, const std::deque<int*>& q) {
        return p.size() < q.size();
      }
  );
  EXPECT_EQ(paths[0], std::deque<int*>({ind[1], ind[3]}));
  EXPECT_EQ(paths[1], std::deque<int*>({ind[1], ind[2], ind[3]}));
  EXPECT_EQ(paths[2], std::deque<int*>({ind[1], ind[5], ind[4], ind[3]}));
  EXPECT_EQ(
      paths[3], std::deque<int*>({
                    ind[1],
                    ind[5],
                    ind[4],
                    ind[2],
                    ind[3],
                })
  );
}
