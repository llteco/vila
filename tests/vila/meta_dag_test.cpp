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
 * Description: test DAGs algorithms
 ****************************************/
#include <gtest/gtest.h>

#include "vila/graph/dag.h"
#include "vila/graph/property.h"

using namespace vila;  // NOLINT(*-using-namespace)

TEST(DiGraphTest, NumberOfCycles) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 12; i++) {  // NOLINT(*-magic-numbers)
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
    *nodes.back().get() = i;
  }
  g.AddNodes(nodes.begin(), nodes.end());
  EXPECT_EQ(KosarajuStronglyConnectedComponents(g).size(), 12);
  g.AddEdge(indices[0], indices[1]);
  g.AddEdge(indices[0], indices[5]);
  g.AddEdge(indices[2], indices[0]);
  g.AddEdge(indices[2], indices[3]);
  g.AddEdge(indices[3], indices[2]);
  g.AddEdge(indices[3], indices[5]);
  g.AddEdge(indices[4], indices[2]);
  g.AddEdge(indices[4], indices[3]);
  g.AddEdge(indices[5], indices[4]);
  g.AddEdge(indices[6], indices[0]);
  g.AddEdge(indices[6], indices[4]);
  g.AddEdge(indices[6], indices[8]);
  g.AddEdge(indices[6], indices[9]);
  g.AddEdge(indices[7], indices[6]);
  g.AddEdge(indices[7], indices[9]);
  g.AddEdge(indices[8], indices[6]);
  g.AddEdge(indices[9], indices[10]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[9], indices[11]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[10], indices[11]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[11], indices[4]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[11], indices[9]);
  auto cycles = SimpleCycles(g);
  EXPECT_EQ(cycles.size(), 8);
  std::sort(
      cycles.begin(), cycles.end(),
      [](const std::unordered_set<int *> &a,
         const std::unordered_set<int *> &b) { return a.size() < b.size(); }
  );
  // the largest cycle is [0,5,4,3,2]
  EXPECT_EQ(cycles.rbegin()->size(), 5);
  EXPECT_EQ(
      *cycles.rbegin(), std::unordered_set<int *>({
                            indices[0],
                            indices[5],
                            indices[4],
                            indices[3],
                            indices[2],
                        })
  );
}

TEST(DiGraphTest, Sort) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 3; i++) {
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
  }
  g.AddNodes(nodes.begin(), nodes.end());
  g.AddEdge(indices[0], indices[1]);
  g.AddEdge(indices[0], indices[2]);
  g.AddEdge(indices[1], indices[2]);
  EXPECT_EQ(
      TopologicalSort(g),
      std::deque<int *>({indices[0], indices[1], indices[2]})
  );
  // cyclic error
  g.AddEdge(indices[2], indices[1]);
  bool error = false;
  TopologicalSort(g, &error);
  EXPECT_TRUE(error);
  // order changes
  g.RemoveEdge(indices[1], indices[2]);
  EXPECT_EQ(
      TopologicalSort(g),
      std::deque<int *>({indices[0], indices[2], indices[1]})
  );
}

TEST(DiGraphTest, AllTopologicalSorts) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.emplace_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(std::move(nodes));
  auto sorts = AllTopologicalSorts(g);
  EXPECT_EQ(sorts.size(), 120);  // all arrangements 5!
  for (const auto &sort : sorts) {
    EXPECT_EQ(sort.size(), 5);
  }

  g.AddEdge(ind[0], ind[2]);
  g.AddEdge(ind[1], ind[0]);
  g.AddEdge(ind[1], ind[3]);
  g.AddEdge(ind[3], ind[2]);
  g.AddEdge(ind[3], ind[4]);
  sorts = AllTopologicalSorts(g);
  EXPECT_EQ(sorts.size(), 5);
  std::deque<std::deque<int *>> expects{
      {ind[1], ind[0], ind[3], ind[2], ind[4]},
      {ind[1], ind[0], ind[3], ind[4], ind[2]},
      {ind[1], ind[3], ind[4], ind[0], ind[2]},
      {ind[1], ind[3], ind[0], ind[2], ind[4]},
      {ind[1], ind[3], ind[0], ind[4], ind[2]},
  };
  while (!sorts.empty()) {
    auto sort = sorts.back();
    sorts.pop_back();
    bool matched = false;
    for (const auto &expect : expects) {
      if (sort == expect) {
        matched = true;
        break;
      }
    }
    EXPECT_TRUE(matched) << "current sort doesn't match";
  }
}

TEST(DiGraphTest, WeightedSort) {
  DiGraph<int, node::color> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> ind;
  for (int i = 0; i < 5; i++) {
    nodes.emplace_back(new int);
    ind.emplace_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(nodes.rbegin(), nodes.rend());
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[3], ind[0]);
  g.AddEdge(ind[2], ind[1]);
  g.AddEdge(ind[1], ind[4]);
  g.SetProperty(ind[0], node::color::red);
  g.SetProperty(ind[1], node::color::red);
  g.SetProperty(ind[2], node::color::blue);
  g.SetProperty(ind[3], node::color::blue);
  g.SetProperty(ind[3], node::color::blue);
  bool error = false;
  auto orders = WeightedTopologicalSort(g, &error);
  EXPECT_FALSE(error);
  std::deque<int *> expected{ind[2], ind[3], ind[1], ind[0], ind[4]};
  if (orders != expected) {
    EXPECT_EQ(
        orders, std::deque<int *>({ind[2], ind[3], ind[0], ind[1], ind[4]})
    );
  }
}

TEST(DiGraphTest, WeightedSortCustom1) {
  DiGraph<int, node::color> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> ind;
  for (int i = 0; i < 7; i++) {
    nodes.emplace_back(new int);
    ind.emplace_back(nodes.back().get());
    *ind.back() = i;
  }
  g.AddNodes(nodes.rbegin(), nodes.rend());
  g.AddEdge(ind[0], ind[1]);
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[0], ind[4]);
  g.AddEdge(ind[4], ind[5]);
  g.AddEdge(ind[5], ind[6]);
  g.SetProperty(ind[0], node::color::black);
  g.SetProperty(ind[1], node::color::black);
  g.SetProperty(ind[3], node::color::black);
  g.SetProperty(ind[5], node::color::black);
  g.SetProperty(ind[2], node::color::yellow);
  g.SetProperty(ind[4], node::color::yellow);
  g.SetProperty(ind[6], node::color::yellow);
  bool error = false;
  auto orders = WeightedTopologicalSort(g, &error);
  EXPECT_FALSE(error);
  std::deque<node::color> color_order;
  std::for_each(orders.begin(), orders.end(), [&](int *it) {
    color_order.push_back(g.GetProperty(it));
  });
  EXPECT_EQ(
      color_order, std::deque<node::color>({
                       node::color::black,
                       node::color::black,
                       node::color::yellow,
                       node::color::yellow,
                       node::color::black,
                       node::color::black,
                       node::color::yellow,
                   })
  );
}

TEST(DiGraphTest, StronglyConnectedComponents) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> indices;
  for (int i = 0; i < 12; i++) {  // NOLINT(*-magic-numbers)
    nodes.emplace_back(new int);
    indices.emplace_back(nodes.back().get());
    *nodes.back().get() = i;
  }
  g.AddNodes(nodes.begin(), nodes.end());
  EXPECT_EQ(KosarajuStronglyConnectedComponents(g).size(), 12);
  g.AddEdge(indices[0], indices[1]);
  g.AddEdge(indices[0], indices[5]);
  g.AddEdge(indices[2], indices[0]);
  g.AddEdge(indices[2], indices[3]);
  g.AddEdge(indices[3], indices[2]);
  g.AddEdge(indices[3], indices[5]);
  g.AddEdge(indices[4], indices[2]);
  g.AddEdge(indices[4], indices[3]);
  g.AddEdge(indices[5], indices[4]);
  g.AddEdge(indices[6], indices[0]);
  g.AddEdge(indices[6], indices[4]);
  g.AddEdge(indices[6], indices[8]);
  g.AddEdge(indices[6], indices[9]);
  g.AddEdge(indices[7], indices[6]);
  g.AddEdge(indices[7], indices[9]);
  g.AddEdge(indices[8], indices[6]);
  g.AddEdge(indices[9], indices[10]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[9], indices[11]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[10], indices[11]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[11], indices[4]);
  // NOLINTNEXTLINE(*-magic-numbers)
  g.AddEdge(indices[11], indices[9]);
  auto scc = KosarajuStronglyConnectedComponents(g);
  ASSERT_EQ(scc.size(), 5);
  EXPECT_EQ(scc[0], std::unordered_set<int *>({indices[1]}));
  EXPECT_EQ(
      scc[1], std::unordered_set<int *>({
                  indices[0],
                  indices[2],
                  indices[3],
                  indices[4],
                  indices[5],
              })
  );
  EXPECT_EQ(
      scc[2], std::unordered_set<int *>({
                  indices[9],
                  indices[10],
                  indices[11],
              })
  );
  EXPECT_EQ(
      scc[3], std::unordered_set<int *>({
                  indices[6],
                  indices[8],
              })
  );
  EXPECT_EQ(scc[4], std::unordered_set<int *>({indices[7]}));
}

TEST(DagTest, LongestPathEmpty) {
  DiGraph<int> g("main");
  EXPECT_EQ(DagLongestPath(g), std::deque<int *>());
}

TEST(DagTest, LongestPathCustom1) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> ind;
  for (int i = 0; i < 12; i++) {  // NOLINT(*-magic-numbers)
    nodes.emplace_back(new int);
    ind.emplace_back(nodes.back().get());
    *nodes.back().get() = i;
  }
  g.AddNodes(nodes.begin(), nodes.end());
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[2], ind[4]);
  g.AddEdge(ind[3], ind[5]);
  g.AddEdge(ind[5], ind[6]);
  g.AddEdge(ind[3], ind[7]);
  EXPECT_EQ(
      DagLongestPath(g),
      std::deque<int *>({ind[1], ind[2], ind[3], ind[5], ind[6]})
  );
}

TEST(DagTest, LongestPathCustom2) {
  DiGraph<int> g("main");
  std::deque<Node<int>> nodes;
  std::deque<int *> ind;
  for (int i = 0; i < 12; i++) {  // NOLINT(*-magic-numbers)
    nodes.emplace_back(new int);
    ind.emplace_back(nodes.back().get());
    *nodes.back().get() = i;
  }
  g.AddNodes(nodes.begin(), nodes.end());
  g.AddEdge(ind[1], ind[2]);
  g.AddEdge(ind[2], ind[3]);
  g.AddEdge(ind[3], ind[4]);
  g.AddEdge(ind[4], ind[5]);
  g.AddEdge(ind[1], ind[3]);
  g.AddEdge(ind[1], ind[5]);
  g.AddEdge(ind[3], ind[5]);
  EXPECT_EQ(
      DagLongestPath(g),
      std::deque<int *>({ind[1], ind[2], ind[3], ind[4], ind[5]})
  );
}
