#ifndef DSA_TREES_PRIORITY_QUEUE_HPP
#define DSA_TREES_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include <utility>

#include "dsa/interfaces/collection.hpp"
#include "dsa/trees/binary_heap.hpp"

namespace dsa {

/**
 * @brief TDA cola de prioridad (priority queue): siempre sirve primero su
 * elemento de "mayor prioridad", donde la prioridad la define `Compare`.
 *
 * Esta clase es intencionalmente un adaptador delgado, no una segunda
 * implementación: una cola de prioridad *es* un montículo binario bajo
 * otro nombre y con una API más reducida (compárese con cómo
 * std::priority_queue, en la biblioteca estándar, adapta un contenedor
 * subyacente, típicamente std::vector, exactamente de la misma manera).
 * Todo el trabajo real —almacenamiento en arreglo, sift_up, sift_down,
 * build_heap en O(n)— vive en BinaryHeap; ver
 * include/dsa/trees/binary_heap.hpp y docs/guides/05_heaps.md.
 *
 * Con el std::less<T> por defecto, los elementos de mayor valor tienen
 * mayor prioridad (una cola de prioridad máxima). Pasar std::greater<T>
 * da una cola de prioridad mínima, exactamente igual que con BinaryHeap y
 * std::priority_queue.
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 * @tparam Compare Functor de orden estricto débil que define el orden de
 * prioridad. Por defecto std::less<T>.
 */
template <typename T, typename Compare = std::less<T>>
class PriorityQueue : public Collection<T> {
   public:
    /** @brief Construye una cola de prioridad vacía. */
    PriorityQueue() = default;

    /** @brief Construye una cola de prioridad vacía con un comparador personalizado. */
    explicit PriorityQueue(const Compare& compare) : heap_(compare) {}

    /**
     * @brief Construye una cola de prioridad a partir del rango
     * `[first, last)` en O(n), delegando en el constructor build_heap de
     * BinaryHeap.
     */
    template <typename InputIt>
    PriorityQueue(InputIt first, InputIt last, const Compare& compare = Compare())
        : heap_(first, last, compare) {}

    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue(PriorityQueue&&) noexcept = default;
    PriorityQueue& operator=(const PriorityQueue&) = default;
    PriorityQueue& operator=(PriorityQueue&&) noexcept = default;
    ~PriorityQueue() override = default;

    /** @brief Número de elementos almacenados actualmente. */
    std::size_t size() const noexcept override { return heap_.size(); }

    /** @brief Indica si la cola de prioridad no contiene elementos. */
    bool empty() const noexcept override { return heap_.empty(); }

    /** @brief Elimina todos los elementos. */
    void clear() override { heap_.clear(); }

    /** @brief Inserta `value` según su prioridad. O(log n) amortizado. */
    void push(const T& value) { heap_.push(value); }
    /** @brief Inserta `value` mediante move. O(log n) amortizado. */
    void push(T&& value) { heap_.push(std::move(value)); }

    /**
     * @brief Elimina y devuelve el elemento de mayor prioridad. O(log n).
     * @throws std::out_of_range si la cola está vacía.
     */
    T pop() { return heap_.pop(); }

    /**
     * @brief Acceso de solo lectura al elemento de mayor prioridad.
     * @throws std::out_of_range si la cola está vacía.
     */
    const T& top() const { return heap_.top(); }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(PriorityQueue& other) noexcept { heap_.swap(other.heap_); }

   private:
    BinaryHeap<T, Compare> heap_;
};

}  // namespace dsa

#endif  // DSA_TREES_PRIORITY_QUEUE_HPP
