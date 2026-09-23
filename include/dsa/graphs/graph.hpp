#ifndef DSA_GRAPHS_GRAPH_HPP
#define DSA_GRAPHS_GRAPH_HPP

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dsa {

/**
 * @brief Un grafo dirigido o no dirigido, ponderado, almacenado como una
 * lista de adyacencia.
 *
 * Graph<Vertex> mantiene un `std::unordered_map<Vertex, std::vector<std::pair<Vertex,
 * double>>>` que asocia cada vértice con su lista de pares (vecino, peso).
 * Esta representación cuesta O(V + E) de memoria y responde "¿quiénes son
 * los vecinos de v?" en tiempo O(deg(v)), que es justo lo que necesitan
 * BFS/DFS/Dijkstra. A cambio, se renuncia al `has_edge` en O(1) que
 * ofrecería una matriz de adyacencia (aquí es O(deg(v))), a cambio de no
 * pagar O(V^2) de memoria en grafos dispersos -- ver docs/guides/07_graphs.md
 * para la discusión completa de esta disyuntiva.
 *
 * El grafo puede construirse como dirigido o no dirigido. En el caso no
 * dirigido, `add_edge(u, v, w)` inserta tanto (u -> v, w) como (v -> u, w);
 * las dos direcciones se almacenan de forma independiente, así que
 * `remove_edge` también elimina ambas.
 *
 * Los pesos de las aristas son 1.0 por defecto, así que un grafo no
 * ponderado puede simplemente ignorar el parámetro de peso y todos los
 * algoritmos se siguen comportando de forma razonable (p. ej. BFS da el
 * camino más corto en número de aristas, y Dijkstra con pesos unitarios
 * coincide con las distancias de BFS).
 *
 * @tparam Vertex Tipo identificador de vértice. Debe poder usarse como
 * clave en `std::unordered_map` (hasheable y comparable por igualdad). Por
 * defecto es `int`.
 */
template <typename Vertex = int>
class Graph {
   public:
    using AdjacencyList = std::vector<std::pair<Vertex, double>>;

    /**
     * @brief Construye un grafo vacío.
     * @param directed Si es true, las aristas son unidireccionales (solo
     * u -> v). Si es false (el valor por defecto), `add_edge`/`remove_edge`
     * operan de forma simétrica.
     */
    explicit Graph(bool directed = false) : directed_(directed), edge_count_(0) {}

    /** @brief True si este grafo trata las aristas como unidireccionales. */
    bool directed() const noexcept { return directed_; }

    /** @brief Agrega `v` al grafo sin vecinos, si no estaba ya presente. */
    void add_vertex(const Vertex& v) { adjacency_.try_emplace(v); }

    /**
     * @brief Agrega una arista ponderada de `u` a `v` (y de `v` a `u` cuando
     * el grafo no es dirigido). Crea `u` y `v` como vértices si aún no
     * existían. No verifica aristas duplicadas: agregar la misma arista dos
     * veces crea una arista paralela.
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
     * @brief Elimina toda arista de `u` a `v` (y de `v` a `u` cuando el
     * grafo no es dirigido). No hace nada si la arista no existe.
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

    /** @brief True si existe al menos una arista de `u` a `v`. */
    bool has_edge(const Vertex& u, const Vertex& v) const {
        auto it = adjacency_.find(u);
        if (it == adjacency_.end()) return false;
        const auto& list = it->second;
        return std::any_of(list.begin(), list.end(),
                           [&v](const std::pair<Vertex, double>& e) { return e.first == v; });
    }

    /**
     * @brief Devuelve los pares (vecino, peso) alcanzables directamente
     * desde `v`. Devuelve un vector vacío si `v` no está en el grafo.
     */
    const AdjacencyList& neighbors(const Vertex& v) const {
        static const AdjacencyList kEmpty{};
        auto it = adjacency_.find(v);
        if (it == adjacency_.end()) return kEmpty;
        return it->second;
    }

    /** @brief True si `v` ha sido agregado al grafo (como vértice o extremo de arista). */
    bool contains(const Vertex& v) const { return adjacency_.find(v) != adjacency_.end(); }

    /** @brief Número de vértices actualmente en el grafo. */
    std::size_t vertex_count() const noexcept { return adjacency_.size(); }

    /**
     * @brief Número de aristas agregadas mediante `add_edge`. En un grafo no
     * dirigido, una arista {u, v} cuenta una sola vez aunque se almacene en
     * ambas listas de adyacencia.
     */
    std::size_t edge_count() const noexcept { return edge_count_; }

    /** @brief Devuelve todos los vértices actualmente en el grafo, en orden no especificado. */
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
