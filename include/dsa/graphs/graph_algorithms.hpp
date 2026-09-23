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
    // TODO(estudiante): usar una cola (std::queue) y un conjunto de
    // visitados (std::unordered_set) para recorrer el grafo nivel a nivel.
    throw std::logic_error("dsa::bfs: no implementado");
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
    // TODO(estudiante): usar una pila explícita (std::vector como stack) y
    // un conjunto de visitados para recorrer el grafo en profundidad.
    throw std::logic_error("dsa::dfs: no implementado");
}

namespace detail {

template <typename Vertex>
void dfs_recursive_impl(const Graph<Vertex>& graph, const Vertex& current,
                        std::unordered_set<Vertex>& visited, std::vector<Vertex>& order) {
    // TODO(estudiante): marcar `current` como visitado, agregarlo a `order`
    // y llamar recursivamente sobre cada vecino no visitado.
    throw std::logic_error("dsa::dfs_recursive: no implementado");
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
    // TODO(estudiante): usar std::priority_queue de pares (distancia,
    // vertice) para relajar aristas por orden creciente de distancia
    // acumulada; verificar primero que no haya pesos negativos.
    throw std::logic_error("dsa::dijkstra: no implementado");
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
    // TODO(estudiante): algoritmo de Kahn con grados de entrada: calcular el
    // in-degree de cada vertice, encolar los que tienen in-degree 0, y al
    // sacarlos de la cola decrementar el in-degree de sus vecinos.
    throw std::logic_error("dsa::topological_sort: no implementado");
}

}  // namespace dsa

#endif  // DSA_GRAPHS_GRAPH_ALGORITHMS_HPP
