#include "dsa/graphs/graph_algorithms.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dsa/graphs/graph.hpp"

namespace {

using dsa::bfs;
using dsa::dfs;
using dsa::dfs_recursive;
using dsa::dijkstra;
using dsa::Graph;
using dsa::topological_sort;

// Auxiliar: índice de la primera aparición de un elemento en `order`, o -1.
template <typename T>
int index_of(const std::vector<T>& order, const T& value) {
    auto it = std::find(order.begin(), order.end(), value);
    return it == order.end() ? -1 : static_cast<int>(it - order.begin());
}

// ---------------------------------------------------------------------
// BFS (recorrido en anchura)
// ---------------------------------------------------------------------

TEST(Bfs, VisitsAllReachableVerticesInLevelOrder) {
    // 0 - 1 - 2
    // |       |
    // 3 ------+   (ciclo: 0-1-2-3-0)
    Graph<int> g;
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 0);

    auto order = bfs(g, 0);
    EXPECT_EQ(order.size(), 4u);
    EXPECT_EQ(order.front(), 0);
    std::unordered_set<int> visited(order.begin(), order.end());
    EXPECT_EQ(visited, (std::unordered_set<int>{0, 1, 2, 3}));
}

TEST(Bfs, OnlyVisitsReachableComponent) {
    Graph<int> g;
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(4, 5);  // Componente desconectada.

    auto order = bfs(g, 1);
    std::unordered_set<int> visited(order.begin(), order.end());
    EXPECT_EQ(visited, (std::unordered_set<int>{1, 2, 3}));
    EXPECT_EQ(visited.count(4), 0u);
    EXPECT_EQ(visited.count(5), 0u);
}

TEST(Bfs, UnknownSourceReturnsEmpty) {
    Graph<int> g;
    g.add_edge(1, 2);
    EXPECT_TRUE(bfs(g, 99).empty());
}

// ---------------------------------------------------------------------
// DFS (recorrido en profundidad)
// ---------------------------------------------------------------------

TEST(Dfs, VisitsAllReachableVerticesWithCycle) {
    Graph<int> g;
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);  // Ciclo.
    g.add_edge(2, 3);

    auto order = dfs(g, 0);
    EXPECT_EQ(order.size(), 4u);
    std::unordered_set<int> visited(order.begin(), order.end());
    EXPECT_EQ(visited, (std::unordered_set<int>{0, 1, 2, 3}));
}

TEST(Dfs, OnlyVisitsReachableComponent) {
    Graph<int> g;
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(4, 5);  // Componente desconectada.

    auto order = dfs(g, 1);
    std::unordered_set<int> visited(order.begin(), order.end());
    EXPECT_EQ(visited, (std::unordered_set<int>{1, 2, 3}));
}

TEST(Dfs, IterativeAndRecursiveAgreeOnVisitedSet) {
    Graph<int> g;
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);

    auto iterative = dfs(g, 0);
    auto recursive = dfs_recursive(g, 0);
    std::unordered_set<int> iterative_set(iterative.begin(), iterative.end());
    std::unordered_set<int> recursive_set(recursive.begin(), recursive.end());
    EXPECT_EQ(iterative_set, recursive_set);
    EXPECT_EQ(iterative.size(), recursive.size());
}

TEST(Dfs, UnknownSourceReturnsEmpty) {
    Graph<int> g;
    g.add_edge(1, 2);
    EXPECT_TRUE(dfs(g, 99).empty());
}

// ---------------------------------------------------------------------
// Dijkstra (camino más corto)
// ---------------------------------------------------------------------

// Grafo no dirigido y ponderado conocido, distancias desde 0 verificadas a mano:
//
//        4
//   0 ------- 1
//   | \      /|
//  1|  \2   /1|
//   |   \  /  |
//   2 ---\--- (via 1)
//    \        |
//    5\      3|
//      \      |
//       3 --- 4
//
// Aristas: (0,1,4) (0,2,1) (1,2,2) (1,3,1) (2,3,5) (3,4,3)
// Distancias más cortas desde 0: 0->0, 1->3 (via 2), 2->1, 3->4 (via 2,1), 4->7.
class DijkstraKnownGraph : public ::testing::Test {
   protected:
    void SetUp() override {
        g.add_edge(0, 1, 4.0);
        g.add_edge(0, 2, 1.0);
        g.add_edge(1, 2, 2.0);
        g.add_edge(1, 3, 1.0);
        g.add_edge(2, 3, 5.0);
        g.add_edge(3, 4, 3.0);
    }
    Graph<int> g;
};

TEST_F(DijkstraKnownGraph, MatchesHandComputedDistances) {
    auto dist = dijkstra(g, 0);
    EXPECT_DOUBLE_EQ(dist.at(0), 0.0);
    EXPECT_DOUBLE_EQ(dist.at(1), 3.0);
    EXPECT_DOUBLE_EQ(dist.at(2), 1.0);
    EXPECT_DOUBLE_EQ(dist.at(3), 4.0);
    EXPECT_DOUBLE_EQ(dist.at(4), 7.0);
}

TEST(Dijkstra, SourceOnlyGraphHasZeroDistanceToItself) {
    Graph<int> g;
    g.add_vertex(0);
    auto dist = dijkstra(g, 0);
    EXPECT_DOUBLE_EQ(dist.at(0), 0.0);
    EXPECT_EQ(dist.size(), 1u);
}

TEST(Dijkstra, UnreachableVertexIsAbsent) {
    Graph<int> g(true);
    g.add_edge(0, 1, 1.0);
    g.add_vertex(2);  // Aislado: inalcanzable desde 0.
    auto dist = dijkstra(g, 0);
    EXPECT_EQ(dist.find(2), dist.end());
}

TEST(Dijkstra, ThrowsOnNegativeWeight) {
    Graph<int> g(true);
    g.add_edge(0, 1, -3.0);
    EXPECT_THROW(dijkstra(g, 0), std::invalid_argument);
}

// ---------------------------------------------------------------------
// Orden topológico
// ---------------------------------------------------------------------

// DAG que modela prerrequisitos de cursos:
//   A -> B, A -> C, B -> D, C -> D, D -> E
TEST(TopologicalSort, RespectsAllDependencies) {
    Graph<char> g(true);
    g.add_edge('A', 'B');
    g.add_edge('A', 'C');
    g.add_edge('B', 'D');
    g.add_edge('C', 'D');
    g.add_edge('D', 'E');

    auto order = topological_sort(g);
    EXPECT_EQ(order.size(), 5u);
    EXPECT_LT(index_of(order, 'A'), index_of(order, 'B'));
    EXPECT_LT(index_of(order, 'A'), index_of(order, 'C'));
    EXPECT_LT(index_of(order, 'B'), index_of(order, 'D'));
    EXPECT_LT(index_of(order, 'C'), index_of(order, 'D'));
    EXPECT_LT(index_of(order, 'D'), index_of(order, 'E'));
}

TEST(TopologicalSort, ThrowsOnCycle) {
    Graph<int> g(true);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 1);  // Ciclo.
    EXPECT_THROW(topological_sort(g), std::runtime_error);
}

TEST(TopologicalSort, ThrowsOnUndirectedGraph) {
    Graph<int> g;  // No dirigido por defecto.
    g.add_edge(1, 2);
    EXPECT_THROW(topological_sort(g), std::invalid_argument);
}

TEST(TopologicalSort, SingleVertexNoEdges) {
    Graph<int> g(true);
    g.add_vertex(1);
    auto order = topological_sort(g);
    ASSERT_EQ(order.size(), 1u);
    EXPECT_EQ(order[0], 1);
}

}  // namespace
