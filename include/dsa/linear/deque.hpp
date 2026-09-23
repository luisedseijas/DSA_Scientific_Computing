#ifndef DSA_LINEAR_DEQUE_HPP
#define DSA_LINEAR_DEQUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Una cola doblemente terminada respaldada por un buffer circular.
 *
 * Deque posee un buffer crudo, asignado en el heap, de capacidad `capacity_`.
 * Los elementos viven en las posiciones `[front_, front_ + size_)`, con
 * índices tomados módulo `capacity_` (el mismo esquema de wraparound que
 * Queue). Llevar un índice `front_` que puede moverse tanto hacia adelante
 * como hacia atrás alrededor del buffer es lo que permite que
 * push_front()/pop_front() y push_back()/pop_back() se ejecuten todos en
 * O(1): ver docs/guides/03_stacks_queues.md para el argumento completo.
 *
 * Cuando el buffer se llena, se asigna un buffer nuevo y más grande y los
 * elementos existentes se copian/mueven a él empezando en el índice 0,
 * exactamente igual que el crecimiento de Queue. Esto le da tanto a
 * push_front() como a push_back() un costo O(1) amortizado.
 *
 * La clase gestiona la memoria manualmente con placement-new/llamadas
 * explícitas al destructor en lugar de envolver std::deque, para que los
 * estudiantes vean exactamente cómo funciona un buffer circular
 * doblemente terminado por dentro.
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 */
template <typename T>
class Deque : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Construye una Deque vacía sin almacenamiento asignado. */
    Deque() noexcept : data_(nullptr), front_(0), size_(0), capacity_(0) {}

    /** @brief Constructor de copia. Realiza una copia profunda de los elementos de `other`. */
    Deque(const Deque& other) : data_(nullptr), front_(0), size_(0), capacity_(0) {
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
    Deque(Deque&& other) noexcept
        : data_(other.data_), front_(other.front_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.front_ = 0;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Operador de asignación por copia (copy-and-swap). */
    Deque& operator=(const Deque& other) {
        if (this != &other) {
            Deque tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Operador de asignación por movimiento. */
    Deque& operator=(Deque&& other) noexcept {
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
    ~Deque() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Número de elementos actualmente almacenados. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Número de elementos que el buffer actual puede contener sin crecer. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Indica si la deque no contiene elementos. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destruye todos los elementos, dejando size() == 0. La capacidad se mantiene. */
    void clear() override {
        destroy_all();
        size_ = 0;
        front_ = 0;
    }

    /** @brief Inserta `value` al frente de la deque. O(1) amortizado. */
    void push_front(const T& value) { push_front_impl(value); }

    /** @brief Inserta `value` al frente de la deque mediante movimiento. O(1) amortizado. */
    void push_front(T&& value) { push_front_impl(std::move(value)); }

    /** @brief Inserta `value` al final de la deque. O(1) amortizado. */
    void push_back(const T& value) { push_back_impl(value); }

    /** @brief Inserta `value` al final de la deque mediante movimiento. O(1) amortizado. */
    void push_back(T&& value) { push_back_impl(std::move(value)); }

    /**
     * @brief Elimina el elemento al frente de la deque. O(1).
     * @throws std::out_of_range si la deque está vacía.
     */
    void pop_front() {
        // TODO(estudiante): destruir el elemento en front_, avanzar front_
        // una posición (con wraparound) y decrementar size_.
        throw std::logic_error("Deque::pop_front: no implementado");
    }

    /**
     * @brief Elimina el elemento al final de la deque. O(1).
     * @throws std::out_of_range si la deque está vacía.
     */
    void pop_back() {
        // TODO(estudiante): destruir el elemento lógico en la posición
        // size_ - 1 (índice físico index_of(size_ - 1)) y decrementar size_.
        throw std::logic_error("Deque::pop_back: no implementado");
    }

    /**
     * @brief Referencia al elemento al frente de la deque.
     * @throws std::out_of_range si la deque está vacía.
     */
    reference front() {
        if (empty()) {
            throw std::out_of_range("Deque::front: la deque está vacía");
        }
        return data_[front_];
    }

    /** @brief Referencia constante al elemento al frente de la deque. */
    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("Deque::front: la deque está vacía");
        }
        return data_[front_];
    }

    /**
     * @brief Referencia al elemento al final de la deque.
     * @throws std::out_of_range si la deque está vacía.
     */
    reference back() {
        if (empty()) {
            throw std::out_of_range("Deque::back: la deque está vacía");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Referencia constante al elemento al final de la deque. */
    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("Deque::back: la deque está vacía");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(Deque& other) noexcept {
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

    /** @brief Resta un paso a `index`, dando la vuelta alrededor de `capacity_`. */
    std::size_t retreat(std::size_t index) const {
        // TODO(estudiante): devolver el índice anterior a `index`,
        // envolviendo a capacity_ - 1 cuando index es 0.
        (void)index;
        throw std::logic_error("Deque::retreat: no implementado");
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
        throw std::logic_error("Deque::grow: no implementado");
    }

    template <typename U>
    void push_front_impl(U&& value) {
        // TODO(estudiante): si el buffer está lleno (size_ == capacity_),
        // llamar a grow(); luego retroceder front_ con retreat() y
        // construir value ahí, e incrementar size_.
        (void)value;
        throw std::logic_error("Deque::push_front: no implementado");
    }

    template <typename U>
    void push_back_impl(U&& value) {
        // TODO(estudiante): si el buffer está lleno (size_ == capacity_),
        // llamar a grow(); luego construir value en la posición lógica
        // "back" (índice físico advance(front_, size_)) e incrementar size_.
        (void)value;
        throw std::logic_error("Deque::push_back: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_DEQUE_HPP
