#ifndef DSA_GRAPHS_GRAPH_HPP
#define DSA_GRAPHS_GRAPH_HPP

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dsa {

/**
 * @brief A directed or undirected, weighted graph stored as an adjacency
 * list.
 *
 * Graph<Vertex> keeps a `std::unordered_map<Vertex, std::vector<std::pair<Vertex,
 * double>>>` mapping every vertex to its list of (neighbor, weight) pairs.
 * This representation costs O(V + E) memory and answers "who are v's
 * neighbors?" in O(deg(v)) time, which is what BFS/DFS/Dijkstra need. It
 * trades away the O(1) `has_edge` you would get from an adjacency matrix
 * (here it is O(deg(v))) in exchange for not paying O(V^2) memory on sparse
 * graphs -- see docs/guides/07_graphs.md for the full trade-off discussion.
 *
 * The graph can be constructed as directed or undirected. In the undirected
 * case, `add_edge(u, v, w)` inserts both (u -> v, w) and (v -> u, w); the
 * two directions are stored independently, so `remove_edge` also removes
 * both.
 *
 * Edge weights default to 1.0, so an unweighted graph can simply ignore the
 * weight parameter and every algorithm still behaves sensibly (e.g. BFS
 * gives shortest path in number of edges, and Dijkstra on unit weights
 * agrees with BFS distances).
 *
 * @tparam Vertex Vertex identifier type. Must be usable as a key in
 * `std::unordered_map` (hashable and equality-comparable). Defaults to
 * `int`.
 */
template <typename Vertex = int>
class Graph {
   public:
    using AdjacencyList = std::vector<std::pair<Vertex, double>>;

    /**
     * @brief Constructs an empty graph.
     * @param directed If true, edges are one-directional (u -> v only). If
     * false (the default), `add_edge`/`remove_edge` operate symmetrically.
     */
    explicit Graph(bool directed = false) : directed_(directed), edge_count_(0) {}

    /** @brief True if this graph treats edges as one-directional. */
    bool directed() const noexcept { return directed_; }

    /** @brief Adds `v` to the graph with no neighbors, if not already present. */
    void add_vertex(const Vertex& v) { adjacency_.try_emplace(v); }

    /**
     * @brief Adds a weighted edge from `u` to `v` (and from `v` to `u` when
     * the graph is undirected). Creates `u` and `v` as vertices if they did
     * not already exist. Does not check for duplicate edges: adding the same
     * edge twice creates a parallel edge.
     */
    void add_edge(const Vertex& u, const Vertex& v, double weight = 1.0) {
        add_vertex(u);
        add_vertex(v);
        adjacency_[u].emplace_back(v, weight);
        ++edge_count_;
        if (!directed_ && !(u == v)) {
            adjacency_[v].emplace_back(u, weight);
        }
    }

    /**
     * @brief Removes every edge from `u` to `v` (and from `v` to `u` when
     * undirected). No-op if the edge does not exist.
     */
    void remove_edge(const Vertex& u, const Vertex& v) {
        auto it = adjacency_.find(u);
        if (it != adjacency_.end()) {
            std::size_t before = it->second.size();
            auto& list = it->second;
            list.erase(
                std::remove_if(list.begin(), list.end(),
                               [&v](const std::pair<Vertex, double>& e) { return e.first == v; }),
                list.end());
            edge_count_ -= (before - list.size());
        }
        if (!directed_ && !(u == v)) {
            auto jt = adjacency_.find(v);
            if (jt != adjacency_.end()) {
                auto& list = jt->second;
                list.erase(std::remove_if(
                               list.begin(), list.end(),
                               [&u](const std::pair<Vertex, double>& e) { return e.first == u; }),
                           list.end());
            }
        }
    }

    /** @brief True if there is at least one edge from `u` to `v`. */
    bool has_edge(const Vertex& u, const Vertex& v) const {
        auto it = adjacency_.find(u);
        if (it == adjacency_.end()) return false;
        const auto& list = it->second;
        return std::any_of(list.begin(), list.end(),
                           [&v](const std::pair<Vertex, double>& e) { return e.first == v; });
    }

    /**
     * @brief Returns the (neighbor, weight) pairs reachable directly from `v`.
     * Returns an empty vector if `v` is not in the graph.
     */
    const AdjacencyList& neighbors(const Vertex& v) const {
        static const AdjacencyList kEmpty{};
        auto it = adjacency_.find(v);
        if (it == adjacency_.end()) return kEmpty;
        return it->second;
    }

    /** @brief True if `v` has been added to the graph (as a vertex or edge endpoint). */
    bool contains(const Vertex& v) const { return adjacency_.find(v) != adjacency_.end(); }

    /** @brief Number of vertices currently in the graph. */
    std::size_t vertex_count() const noexcept { return adjacency_.size(); }

    /**
     * @brief Number of edges added via `add_edge`. In an undirected graph,
     * an edge {u, v} counts once even though it is stored in both adjacency
     * lists.
     */
    std::size_t edge_count() const noexcept { return edge_count_; }

    /** @brief Returns all vertices currently in the graph, in unspecified order. */
    std::vector<Vertex> vertices() const {
        std::vector<Vertex> out;
        out.reserve(adjacency_.size());
        for (const auto& [vertex, _] : adjacency_) {
            out.push_back(vertex);
        }
        return out;
    }

   private:
    std::unordered_map<Vertex, AdjacencyList> adjacency_;
    bool directed_;
    std::size_t edge_count_;
};

}  // namespace dsa

#endif  // DSA_GRAPHS_GRAPH_HPP
