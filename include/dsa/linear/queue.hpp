#ifndef DSA_LINEAR_QUEUE_HPP
#define DSA_LINEAR_QUEUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Una cola FIFO (first-in, first-out) respaldada por un buffer circular.
 *
 * Queue posee un buffer crudo, asignado en el heap, de capacidad `capacity_`.
 * Los elementos viven en las posiciones `[front_, front_ + size_)`, con
 * índices tomados módulo `capacity_` ("dando la vuelta" al final del buffer
 * de regreso al índice 0). Llevar un índice `front_` en lugar de mantener
 * siempre el primer elemento en el índice 0 es lo que hace que dequeue() sea
 * O(1): una cola respaldada por un arreglo ingenuo que desplaza cada
 * elemento restante hacia la izquierda tras un dequeue es O(n). Ver
 * docs/guides/03_stacks_queues.md para el argumento completo y una tabla de
 * Big-O.
 *
 * Cuando el buffer se llena, se asigna un buffer nuevo y más grande y los
 * elementos existentes se copian/mueven a él empezando en el índice 0 (el
 * wraparound se "desenrolla" durante la copia), exactamente igual que el
 * crecimiento de un arreglo dinámico. Esto le da a enqueue() un costo O(1)
 * amortizado.
 *
 * La clase gestiona la memoria manualmente con placement-new/llamadas
 * explícitas al destructor en lugar de envolver std::vector o std::deque,
 * para que los estudiantes vean exactamente cómo funciona un buffer
 * circular por dentro.
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 */
template <typename T>
class Queue : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Construye una Queue vacía sin almacenamiento asignado. */
    Queue() noexcept : data_(nullptr), front_(0), size_(0), capacity_(0) {}

    /** @brief Constructor de copia. Realiza una copia profunda de los elementos de `other`. */
    Queue(const Queue& other) : data_(nullptr), front_(0), size_(0), capacity_(0) {
        if (other.capacity_ > 0) {
            data_ = allocate(other.capacity_);
            capacity_ = other.capacity_;
        }
        for (std::size_t i = 0; i < other.size_; ++i) {
            new (data_ + i) T(other.data_[other.index_of(i)]);
        }
        size_ = other.size_;
        front_ = 0;
    }

    /** @brief Constructor de movimiento. Roba el buffer de `other`, dejándolo vacío. */
    Queue(Queue&& other) noexcept
        : data_(other.data_), front_(other.front_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.front_ = 0;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Operador de asignación por copia (copy-and-swap). */
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            Queue tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Operador de asignación por movimiento. */
    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            destroy_all();
            deallocate(data_);
            data_ = other.data_;
            front_ = other.front_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.front_ = 0;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    /** @brief Destruye todos los elementos y libera el almacenamiento subyacente. */
    ~Queue() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Número de elementos actualmente almacenados. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Número de elementos que el buffer actual puede contener sin crecer. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Indica si la cola no contiene elementos. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destruye todos los elementos, dejando size() == 0. La capacidad se mantiene. */
    void clear() override {
        destroy_all();
        size_ = 0;
        front_ = 0;
    }

    /** @brief Inserta `value` al final de la cola. O(1) amortizado. */
    void enqueue(const T& value) { enqueue_impl(value); }

    /** @brief Inserta `value` al final de la cola mediante movimiento. O(1) amortizado. */
    void enqueue(T&& value) { enqueue_impl(std::move(value)); }

    /**
     * @brief Elimina el elemento al frente de la cola. O(1).
     * @throws std::out_of_range si la cola está vacía.
     */
    void dequeue() {
        // TODO(estudiante): destruir el elemento en front_, avanzar front_
        // una posición (con wraparound) y decrementar size_.
        throw std::logic_error("Queue::dequeue: no implementado");
    }

    /**
     * @brief Referencia al elemento al frente de la cola.
     * @throws std::out_of_range si la cola está vacía.
     */
    reference front() {
        if (empty()) {
            throw std::out_of_range("Queue::front: la cola está vacía");
        }
        return data_[front_];
    }

    /** @brief Referencia constante al elemento al frente de la cola. */
    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("Queue::front: la cola está vacía");
        }
        return data_[front_];
    }

    /**
     * @brief Referencia al elemento al final de la cola.
     * @throws std::out_of_range si la cola está vacía.
     */
    reference back() {
        if (empty()) {
            throw std::out_of_range("Queue::back: la cola está vacía");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Referencia constante al elemento al final de la cola. */
    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("Queue::back: la cola está vacía");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(Queue& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(front_, other.front_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

   private:
    T* data_;
    std::size_t front_;
    std::size_t size_;
    std::size_t capacity_;

    static T* allocate(std::size_t n) { return static_cast<T*>(::operator new(n * sizeof(T))); }

    static void deallocate(T* ptr) { ::operator delete(ptr); }

    /** @brief Índice físico del buffer para la posición lógica `offset` desde front_. */
    std::size_t index_of(std::size_t offset) const { return advance(front_, offset); }

    /** @brief Suma `steps` a `index`, dando la vuelta alrededor de `capacity_`. */
    std::size_t advance(std::size_t index, std::size_t steps) const {
        return (index + steps) % capacity_;
    }

    void destroy_all() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[index_of(i)].~T();
        }
    }

    /** @brief Duplica la capacidad (o asigna 1 espacio si está vacía actualmente), desenrollando el
     * wraparound. */
    void grow() {
        // TODO(estudiante): asignar un buffer nuevo del doble de capacidad
        // (o 1 si capacity_ es 0), copiar/mover los elementos existentes
        // "desenrollando" el wraparound (empezando en el índice 0 del nuevo
        // buffer), destruirlos en el viejo, liberar el viejo buffer y
        // actualizar data_/capacity_/front_ (front_ vuelve a 0).
        throw std::logic_error("Queue::grow: no implementado");
    }

    template <typename U>
    void enqueue_impl(U&& value) {
        // TODO(estudiante): si el buffer está lleno (size_ == capacity_),
        // llamar a grow(); luego construir value en la posición lógica
        // "back" (índice físico advance(front_, size_)) e incrementar size_.
        (void)value;
        throw std::logic_error("Queue::enqueue: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_QUEUE_HPP
