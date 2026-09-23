#ifndef DSA_LINEAR_STACK_HPP
#define DSA_LINEAR_STACK_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Una pila LIFO (last-in, first-out) respaldada por su propio arreglo dinámico.
 *
 * Stack posee un buffer crudo, asignado en el heap, de capacidad `capacity_`
 * que almacena `size_` elementos construidos al frente, exactamente igual
 * que DynamicArray. El crecimiento es geométrico (la capacidad se duplica
 * cuando se llena), lo que le da a push() un costo O(1) amortizado. Ver
 * docs/guides/03_stacks_queues.md para la teoría completa (disciplina LIFO,
 * casos de uso, tabla de Big-O).
 *
 * La clase gestiona la memoria manualmente con placement-new/llamadas
 * explícitas al destructor en lugar de envolver std::vector, para que los
 * estudiantes vean exactamente cómo se implementa una pila sobre un buffer
 * crudo (asignación, crecimiento, Regla de los Cinco).
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 */
template <typename T>
class Stack : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Construye una Stack vacía sin almacenamiento asignado. */
    Stack() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    /** @brief Constructor de copia. Realiza una copia profunda de los elementos de `other`. */
    Stack(const Stack& other) : data_(nullptr), size_(0), capacity_(0) {
        if (other.capacity_ > 0) {
            data_ = allocate(other.capacity_);
            capacity_ = other.capacity_;
        }
        for (std::size_t i = 0; i < other.size_; ++i) {
            new (data_ + i) T(other.data_[i]);
        }
        size_ = other.size_;
    }

    /** @brief Constructor de movimiento. Roba el buffer de `other`, dejándolo vacío. */
    Stack(Stack&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Operador de asignación por copia (copy-and-swap). */
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            Stack tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Operador de asignación por movimiento. */
    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            destroy_all();
            deallocate(data_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    /** @brief Destruye todos los elementos y libera el almacenamiento subyacente. */
    ~Stack() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Número de elementos actualmente almacenados. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Número de elementos que el buffer actual puede contener sin crecer. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Indica si la pila no contiene elementos. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destruye todos los elementos, dejando size() == 0. La capacidad se mantiene. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /** @brief Inserta `value` en el tope de la pila. O(1) amortizado. */
    void push(const T& value) { push_impl(value); }

    /** @brief Inserta `value` en el tope de la pila mediante movimiento. O(1) amortizado. */
    void push(T&& value) { push_impl(std::move(value)); }

    /**
     * @brief Elimina el elemento en el tope de la pila. O(1).
     * @throws std::out_of_range si la pila está vacía.
     */
    void pop() {
        // TODO(estudiante): quitar el elemento del tope: decrementar size_ y
        // llamar al destructor del elemento que queda fuera de rango.
        throw std::logic_error("Stack::pop: no implementado");
    }

    /**
     * @brief Referencia al elemento en el tope de la pila.
     * @throws std::out_of_range si la pila está vacía.
     */
    reference top() {
        if (empty()) {
            throw std::out_of_range("Stack::top: la pila está vacía");
        }
        return data_[size_ - 1];
    }

    /** @brief Referencia constante al elemento en el tope de la pila. */
    const_reference top() const {
        if (empty()) {
            throw std::out_of_range("Stack::top: la pila está vacía");
        }
        return data_[size_ - 1];
    }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(Stack& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

   private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;

    static T* allocate(std::size_t n) { return static_cast<T*>(::operator new(n * sizeof(T))); }

    static void deallocate(T* ptr) { ::operator delete(ptr); }

    void destroy_all() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
    }

    /** @brief Duplica la capacidad (o asigna 1 espacio si está vacía actualmente). */
    void grow() {
        // TODO(estudiante): asignar un buffer nuevo del doble de capacidad
        // (o 1 si capacity_ es 0), mover los elementos existentes al nuevo
        // buffer, destruirlos en el viejo, liberar el viejo buffer y
        // actualizar data_/capacity_.
        throw std::logic_error("Stack::grow: no implementado");
    }

    template <typename U>
    void push_impl(U&& value) {
        // TODO(estudiante): si el buffer está lleno (size_ == capacity_),
        // llamar a grow(); luego construir value al final del buffer con
        // placement-new e incrementar size_.
        (void)value;
        throw std::logic_error("Stack::push: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_STACK_HPP
