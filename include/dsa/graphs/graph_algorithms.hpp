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
 * @brief Recorrido en anchura (breadth-first) que comienza en `source`.
 *
 * Visita `source` y todo vértice alcanzable desde él, nivel por nivel,
 * usando una cola explícita. Los vértices en otra componente conexa (o, en
 * un grafo dirigido, no alcanzables mediante aristas dirigidas desde
 * `source`) nunca son visitados. Tiempo O(V + E), espacio extra O(V).
 *
 * @return El orden en que los vértices fueron visitados por primera vez.
 * Vacío si `source` no es un vértice de `graph`.
 */
template <typename Vertex>
std::vector<Vertex> bfs(const Graph<Vertex>& graph, const Vertex& source) {
    // TODO(estudiante): usar una cola (std::queue) y un conjunto de
    // visitados (std::unordered_set) para recorrer el grafo nivel a nivel.
    throw std::logic_error("dsa::bfs: no implementado");
}

/**
 * @brief Recorrido en profundidad (depth-first) que comienza en `source`,
 * de forma iterativa (pila explícita, sin recursión) para que no pueda
 * desbordar la pila de llamadas en grafos con caminos largos.
 *
 * Nótese que esta versión iterativa, al apilar todos los vecinos antes de
 * desapilar, no necesariamente visita los vecinos en el mismo orden de
 * izquierda a derecha que lo haría un DFS recursivo ingenuo; ambos son
 * órdenes válidos de profundidad. Se ofrece un auxiliar recursivo como
 * `dfs_recursive` para comparación en clase -- prefiera el `dfs` iterativo
 * en código de producción.
 *
 * Tiempo O(V + E), espacio extra O(V).
 *
 * @return El orden en que los vértices fueron visitados por primera vez.
 * Vacío si `source` no es un vértice de `graph`.
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
 * @brief Recorrido en profundidad recursivo de libro de texto, ofrecido
 * puramente para comparación pedagógica frente al `dfs` iterativo. Cada
 * llamada recursiva consume un marco de pila, así que puede desbordarse en
 * grafos con caminos muy largos -- prefiera `dfs` fuera del salón de clase.
 */
template <typename Vertex>
std::vector<Vertex> dfs_recursive(const Graph<Vertex>& graph, const Vertex& source) {
    std::vector<Vertex> order;
    std::unordered_set<Vertex> visited;
    detail::dfs_recursive_impl(graph, source, visited, order);
    return order;
}

/**
 * @brief Algoritmo de Dijkstra de caminos más cortos desde un único origen.
 *
 * Calcula el peso total mínimo de aristas desde `source` hasta cada vértice
 * alcanzable desde él. Usa `std::priority_queue` (un heap binario) como la
 * cola de prioridad mínima por distancia de la frontera; una futura
 * iteración de este curso podría reemplazarla por `dsa::PriorityQueue` /
 * `dsa::BinaryHeap` del módulo de heaps una vez que ese módulo exista, sin
 * cambiar la interfaz de esta función.
 *
 * Lanza `std::invalid_argument` si alguna arista del grafo tiene peso
 * negativo, ya que la relajación voraz de Dijkstra no es correcta en ese
 * caso (ver docs/guides/07_graphs.md para el porqué, y Bellman-Ford como la
 * solución).
 *
 * Tiempo O((V + E) log V) con un heap binario.
 *
 * @return Mapa de cada vértice alcanzable desde `source` a su distancia más
 * corta. `source` en sí se mapea a 0. Los vértices inalcanzables están
 * ausentes del mapa.
 */
template <typename Vertex>
std::unordered_map<Vertex, double> dijkstra(const Graph<Vertex>& graph, const Vertex& source) {
    // TODO(estudiante): usar std::priority_queue de pares (distancia,
    // vertice) para relajar aristas por orden creciente de distancia
    // acumulada; verificar primero que no haya pesos negativos.
    throw std::logic_error("dsa::dijkstra: no implementado");
}

/**
 * @brief Orden topológico de un grafo acíclico dirigido (DAG) mediante el
 * algoritmo de Kahn: eliminar repetidamente un vértice con grado de entrada
 * 0.
 *
 * Solo tiene sentido para grafos dirigidos. Lanza `std::invalid_argument`
 * si `graph.directed()` es false, y `std::runtime_error` si el grafo
 * contiene un ciclo (en cuyo caso no existe un orden topológico válido --
 * el algoritmo de Kahn detecta esto porque algunos vértices nunca alcanzan
 * grado de entrada 0).
 *
 * Tiempo O(V + E).
 *
 * @return Un orden topológico válido: para cada arista u -> v, u aparece
 * antes que v.
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
