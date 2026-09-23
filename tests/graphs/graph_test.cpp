#include "dsa/graphs/graph.hpp"

#include <gtest/gtest.h>

#include <string>

namespace {

using dsa::Graph;

// ---------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------

TEST(GraphConstruction, DefaultIsUndirectedAndEmpty) {
    Graph<int> g;
    EXPECT_FALSE(g.directed());
    EXPECT_EQ(g.vertex_count(), 0u);
    EXPECT_EQ(g.edge_count(), 0u);
}

TEST(GraphConstruction, DirectedFlagIsRespected) {
    Graph<int> g(true);
    EXPECT_TRUE(g.directed());
}

TEST(GraphConstruction, AddVertexIsIdempotent) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(1);
    EXPECT_EQ(g.vertex_count(), 1u);
}

// ---------------------------------------------------------------------
// add_edge / remove_edge / has_edge (undirected)
// ---------------------------------------------------------------------

TEST(GraphUndirected, AddEdgeIsSymmetric) {
    Graph<int> g;
    g.add_edge(1, 2, 3.5);
    EXPECT_TRUE(g.has_edge(1, 2));
    EXPECT_TRUE(g.has_edge(2, 1));
    EXPECT_EQ(g.vertex_count(), 2u);
    EXPECT_EQ(g.edge_count(), 1u);
}

TEST(GraphUndirected, DefaultWeightIsOne) {
    Graph<int> g;
    g.add_edge(1, 2);
    const auto& neighbors = g.neighbors(1);
    ASSERT_EQ(neighbors.size(), 1u);
    EXPECT_DOUBLE_EQ(neighbors[0].second, 1.0);
}

TEST(GraphUndirected, RemoveEdgeRemovesBothDirections) {
    Graph<int> g;
    g.add_edge(1, 2);
    g.remove_edge(1, 2);
    EXPECT_FALSE(g.has_edge(1, 2));
    EXPECT_FALSE(g.has_edge(2, 1));
}

TEST(GraphUndirected, RemoveNonexistentEdgeIsNoOp) {
    Graph<int> g;
    g.add_vertex(1);
    EXPECT_NO_THROW(g.remove_edge(1, 2));
}

// ---------------------------------------------------------------------
// add_edge / remove_edge / has_edge (directed)
// ---------------------------------------------------------------------

TEST(GraphDirected, AddEdgeIsOneDirectional) {
    Graph<int> g(true);
    g.add_edge(1, 2);
    EXPECT_TRUE(g.has_edge(1, 2));
    EXPECT_FALSE(g.has_edge(2, 1));
}

TEST(GraphDirected, RemoveEdgeOnlyRemovesThatDirection) {
    Graph<int> g(true);
    g.add_edge(1, 2);
    g.add_edge(2, 1);
    g.remove_edge(1, 2);
    EXPECT_FALSE(g.has_edge(1, 2));
    EXPECT_TRUE(g.has_edge(2, 1));
}

// ---------------------------------------------------------------------
// neighbors / vertices / counts
// ---------------------------------------------------------------------

TEST(GraphQueries, NeighborsOfUnknownVertexIsEmpty) {
    Graph<int> g;
    EXPECT_TRUE(g.neighbors(42).empty());
}

TEST(GraphQueries, NeighborsReturnsAllAdjacentPairs) {
    Graph<int> g(true);
    g.add_edge(1, 2, 1.0);
    g.add_edge(1, 3, 2.0);
    const auto& neighbors = g.neighbors(1);
    ASSERT_EQ(neighbors.size(), 2u);
}

TEST(GraphQueries, VerticesReturnsEveryAddedVertex) {
    Graph<std::string> g;
    g.add_vertex("a");
    g.add_edge("b", "c");
    auto vs = g.vertices();
    EXPECT_EQ(vs.size(), 3u);
}

TEST(GraphQueries, EdgeCountCountsUndirectedEdgeOnce) {
    Graph<int> g;
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    EXPECT_EQ(g.edge_count(), 2u);
}

TEST(GraphQueries, ContainsReflectsAddedVertices) {
    Graph<int> g;
    g.add_edge(1, 2);
    EXPECT_TRUE(g.contains(1));
    EXPECT_FALSE(g.contains(99));
}

TEST(GraphQueries, WorksWithStringVertices) {
    Graph<std::string> g(true);
    g.add_edge("Madrid", "Bogota", 8000.0);
    EXPECT_TRUE(g.has_edge("Madrid", "Bogota"));
    EXPECT_FALSE(g.has_edge("Bogota", "Madrid"));
}

}  // namespace
