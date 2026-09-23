#ifndef DSA_GRAPHS_GRAPH_ALGORITHMS_HPP
#define DSA_GRAPHS_GRAPH_ALGORITHMS_HPP

#include <limits>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dsa/graphs/graph.hpp"

namespace dsa {

/**
 * @brief Breadth-first traversal starting at `source`.
 *
 * Visits `source` and every vertex reachable from it, level by level, using
 * an explicit queue. Vertices in a different connected component (or, for a
 * directed graph, not reachable via directed edges from `source`) are never
 * visited. O(V + E) time, O(V) extra space.
 *
 * @return The order in which vertices were first visited. Empty if `source`
 * is not a vertex of `graph`.
 */
template <typename Vertex>
std::vector<Vertex> bfs(const Graph<Vertex>& graph, const Vertex& source) {
    std::vector<Vertex> order;
    if (!graph.contains(source)) return order;

    std::unordered_set<Vertex> visited;
    std::queue<Vertex> pending;

    visited.insert(source);
    pending.push(source);

    while (!pending.empty()) {
        Vertex current = pending.front();
        pending.pop();
        order.push_back(current);
        for (const auto& [neighbor, weight] : graph.neighbors(current)) {
            (void)weight;
            if (visited.insert(neighbor).second) {
                pending.push(neighbor);
            }
        }
    }
    return order;
}

/**
 * @brief Depth-first traversal starting at `source`, iterative (explicit
 * stack, no recursion) so it cannot overflow the call stack on graphs with
 * long paths.
 *
 * Note this iterative version, because it pushes all neighbors before
 * popping, does not necessarily visit neighbors in the same left-to-right
 * order that a naive recursive DFS would; both are valid depth-first
 * orders. A recursive auxiliary is provided as `dfs_recursive` for
 * comparison in class -- prefer the iterative `dfs` in production code.
 *
 * O(V + E) time, O(V) extra space.
 *
 * @return The order in which vertices were first visited. Empty if `source`
 * is not a vertex of `graph`.
 */
template <typename Vertex>
std::vector<Vertex> dfs(const Graph<Vertex>& graph, const Vertex& source) {
    std::vector<Vertex> order;
    if (!graph.contains(source)) return order;

    std::unordered_set<Vertex> visited;
    std::vector<Vertex> stack;
    stack.push_back(source);

    while (!stack.empty()) {
        Vertex current = stack.back();
        stack.pop_back();
        if (!visited.insert(current).second) continue;
        order.push_back(current);
        for (const auto& [neighbor, weight] : graph.neighbors(current)) {
            (void)weight;
            if (visited.find(neighbor) == visited.end()) {
                stack.push_back(neighbor);
            }
        }
    }
    return order;
}

namespace detail {

template <typename Vertex>
void dfs_recursive_impl(const Graph<Vertex>& graph, const Vertex& current,
                        std::unordered_set<Vertex>& visited, std::vector<Vertex>& order) {
    visited.insert(current);
    order.push_back(current);
    for (const auto& [neighbor, weight] : graph.neighbors(current)) {
        (void)weight;
        if (visited.find(neighbor) == visited.end()) {
            dfs_recursive_impl(graph, neighbor, visited, order);
        }
    }
}

}  // namespace detail

/**
 * @brief Textbook recursive depth-first traversal, offered purely for
 * pedagogical comparison against the iterative `dfs`. Each recursive call
 * consumes a stack frame, so this can overflow on graphs with very long
 * paths -- prefer `dfs` outside the classroom.
 */
template <typename Vertex>
std::vector<Vertex> dfs_recursive(const Graph<Vertex>& graph, const Vertex& source) {
    std::vector<Vertex> order;
    std::unordered_set<Vertex> visited;
    detail::dfs_recursive_impl(graph, source, visited, order);
    return order;
}

/**
 * @brief Dijkstra's single-source shortest path algorithm.
 *
 * Computes the minimum total edge weight from `source` to every vertex
 * reachable from it. Uses `std::priority_queue` (a binary heap) as the
 * frontier's min-priority-queue-by-distance; a future iteration of this
 * course can swap it for `dsa::PriorityQueue` / `dsa::BinaryHeap` from the
 * heaps module once that module exists, without changing this function's
 * interface.
 *
 * Throws `std::invalid_argument` if any edge in the graph has a negative
 * weight, since Dijkstra's greedy relaxation is not correct in that case
 * (see docs/guides/07_graphs.md for why, and Bellman-Ford as the fix).
 *
 * O((V + E) log V) time with a binary heap.
 *
 * @return Map from every vertex reachable from `source` to its shortest
 * distance. `source` itself maps to 0. Unreachable vertices are absent from
 * the map.
 */
template <typename Vertex>
std::unordered_map<Vertex, double> dijkstra(const Graph<Vertex>& graph, const Vertex& source) {
    for (const auto& vertex : graph.vertices()) {
        for (const auto& [neighbor, weight] : graph.neighbors(vertex)) {
            (void)neighbor;
            if (weight < 0.0) {
                throw std::invalid_argument(
                    "dijkstra: negative edge weight detected; use Bellman-Ford instead");
            }
        }
    }

    std::unordered_map<Vertex, double> distance;
    using DistVertex = std::pair<double, Vertex>;
    std::priority_queue<DistVertex, std::vector<DistVertex>, std::greater<DistVertex>> frontier;

    distance[source] = 0.0;
    frontier.emplace(0.0, source);

    while (!frontier.empty()) {
        auto [dist_u, u] = frontier.top();
        frontier.pop();

        // Stale entry: a shorter path to u was already relaxed.
        auto it = distance.find(u);
        if (it != distance.end() && dist_u > it->second) continue;

        for (const auto& [v, weight] : graph.neighbors(u)) {
            double candidate = dist_u + weight;
            auto existing = distance.find(v);
            if (existing == distance.end() || candidate < existing->second) {
                distance[v] = candidate;
                frontier.emplace(candidate, v);
            }
        }
    }

    return distance;
}

/**
 * @brief Topological sort of a directed acyclic graph (DAG) via Kahn's
 * algorithm: repeatedly remove a vertex with in-degree 0.
 *
 * Only meaningful for directed graphs. Throws `std::invalid_argument` if
 * `graph.directed()` is false, and `std::runtime_error` if the graph
 * contains a cycle (in which case no valid topological order exists --
 * Kahn's algorithm detects this because some vertices never reach
 * in-degree 0).
 *
 * O(V + E) time.
 *
 * @return A valid topological order: for every edge u -> v, u appears
 * before v.
 */
template <typename Vertex>
std::vector<Vertex> topological_sort(const Graph<Vertex>& graph) {
    if (!graph.directed()) {
        throw std::invalid_argument("topological_sort: graph must be directed");
    }

    std::unordered_map<Vertex, int> in_degree;
    for (const auto& vertex : graph.vertices()) {
        in_degree.try_emplace(vertex, 0);
    }
    for (const auto& vertex : graph.vertices()) {
        for (const auto& [neighbor, weight] : graph.neighbors(vertex)) {
            (void)weight;
            ++in_degree[neighbor];
        }
    }

    std::queue<Vertex> zero_in_degree;
    for (const auto& [vertex, degree] : in_degree) {
        if (degree == 0) zero_in_degree.push(vertex);
    }

    std::vector<Vertex> order;
    order.reserve(graph.vertex_count());

    while (!zero_in_degree.empty()) {
        Vertex current = zero_in_degree.front();
        zero_in_degree.pop();
        order.push_back(current);
        for (const auto& [neighbor, weight] : graph.neighbors(current)) {
            (void)weight;
            if (--in_degree[neighbor] == 0) {
                zero_in_degree.push(neighbor);
            }
        }
    }

    if (order.size() != graph.vertex_count()) {
        throw std::runtime_error("topological_sort: graph contains a cycle");
    }

    return order;
}

}  // namespace dsa

#endif  // DSA_GRAPHS_GRAPH_ALGORITHMS_HPP
