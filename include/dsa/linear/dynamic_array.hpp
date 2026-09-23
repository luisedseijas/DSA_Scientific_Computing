#ifndef DSA_LINEAR_DYNAMIC_ARRAY_HPP
#define DSA_LINEAR_DYNAMIC_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Un arreglo dinámico (arreglo redimensionable de almacenamiento
 * contiguo), similar en espíritu a std::vector.
 *
 * DynamicArray es dueño de un buffer crudo, asignado en el heap, de
 * capacidad `capacity_` que almacena `size_` elementos construidos al
 * frente. Cuando el buffer se llena, se asigna un nuevo buffer con el
 * doble de capacidad y los elementos existentes se mueven (o copian) a
 * él. Esta estrategia de crecimiento geométrico es lo que le da a
 * push_back su costo amortizado O(1): ver docs/guides/01_dynamic_array.md
 * para el argumento completo de análisis amortizado.
 *
 * La clase administra la memoria manualmente con `new`/`delete` a
 * propósito, en lugar de envolver un std::vector, para que los
 * estudiantes puedan ver exactamente cómo se implementa un arreglo
 * dinámico por dentro (asignación, colocación de elementos, crecimiento
 * y la Regla de los Cinco).
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 */
template <typename T>
class DynamicArray : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;

    /** @brief Construye un DynamicArray vacío sin almacenamiento asignado. */
    DynamicArray() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    /**
     * @brief Construye un DynamicArray vacío que ya reservó
     * almacenamiento para `initial_capacity` elementos.
     * @param initial_capacity Número de elementos de almacenamiento a
     * preasignar.
     */
    explicit DynamicArray(std::size_t initial_capacity) : data_(nullptr), size_(0), capacity_(0) {
        if (initial_capacity > 0) {
            data_ = allocate(initial_capacity);
            capacity_ = initial_capacity;
        }
    }

    /** @brief Constructor de copia. Realiza una copia profunda de los elementos de `other`. */
    DynamicArray(const DynamicArray& other) : data_(nullptr), size_(0), capacity_(0) {
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
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Operador de asignación por copia (copy-and-swap). */
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Operador de asignación por movimiento. */
    DynamicArray& operator=(DynamicArray&& other) noexcept {
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
    ~DynamicArray() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Número de elementos actualmente almacenados. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Número de elementos que el buffer actual puede contener sin crecer. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Indica si el arreglo no contiene elementos. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destruye cada elemento, dejando size() == 0. Se conserva la capacidad. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /**
     * @brief Agrega `value` al final, creciendo el buffer si es necesario.
     * Amortizado O(1).
     */
    void push_back(const T& value) { emplace_back_impl(value); }

    /** @brief Agrega `value` al final mediante movimiento. Amortizado O(1). */
    void push_back(T&& value) { emplace_back_impl(std::move(value)); }

    /**
     * @brief Elimina el último elemento.
     * @throws std::out_of_range si el arreglo está vacío.
     */
    void pop_back() {
        // TODO(estudiante): eliminar el último elemento (verificar que no
        // esté vacío, decrementar size_ y llamar al destructor en el slot
        // eliminado).
        throw std::logic_error("DynamicArray::pop_back: no implementado");
    }

    /**
     * @brief Inserta `value` en `index`, desplazando los elementos
     * siguientes a la derecha. O(n) en el peor caso.
     * @param index Posición en la que insertar; debe cumplir 0 <= index <=
     * size().
     * @throws std::out_of_range si index > size().
     */
    void insert(std::size_t index, const T& value) {
        (void)index;
        (void)value;
        // TODO(estudiante): insertar `value` en `index`, creciendo el
        // buffer si es necesario y desplazando los elementos [index,
        // size_) un slot a la derecha para que ningún elemento vivo se
        // sobrescriba antes de leerse.
        throw std::logic_error("DynamicArray::insert: no implementado");
    }

    /**
     * @brief Elimina el elemento en `index`, desplazando los elementos
     * siguientes a la izquierda. O(n) en el peor caso.
     * @throws std::out_of_range si index >= size().
     */
    void erase(std::size_t index) {
        (void)index;
        // TODO(estudiante): eliminar el elemento en `index`, desplazando
        // los elementos siguientes un slot a la izquierda y destruyendo
        // el último slot que queda sin usar.
        throw std::logic_error("DynamicArray::erase: no implementado");
    }

    /** @brief Acceso a elemento sin verificación. Comportamiento indefinido si está fuera de rango.
     */
    reference operator[](std::size_t index) { return data_[index]; }

    /** @brief Acceso const a elemento sin verificación. */
    const_reference operator[](std::size_t index) const { return data_[index]; }

    /**
     * @brief Acceso a elemento con verificación de límites.
     * @throws std::out_of_range si index >= size().
     */
    reference at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray::at: índice fuera de rango");
        }
        return data_[index];
    }

    /** @brief Acceso const a elemento con verificación de límites. */
    const_reference at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray::at: índice fuera de rango");
        }
        return data_[index];
    }

    /** @brief Referencia al primer elemento. Comportamiento indefinido si está vacío. */
    reference front() { return data_[0]; }
    /** @brief Referencia const al primer elemento. */
    const_reference front() const { return data_[0]; }

    /** @brief Referencia al último elemento. Comportamiento indefinido si está vacío. */
    reference back() { return data_[size_ - 1]; }
    /** @brief Referencia const al último elemento. */
    const_reference back() const { return data_[size_ - 1]; }

    /** @brief Puntero al almacenamiento contiguo subyacente. */
    T* data() noexcept { return data_; }
    /** @brief Puntero const al almacenamiento contiguo subyacente. */
    const T* data() const noexcept { return data_; }

    // -- Iteradores, que habilitan range-for y compatibilidad con <algorithm>. --------

    iterator begin() noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator end() const noexcept { return data_ + size_; }
    const_iterator cbegin() const noexcept { return data_; }
    const_iterator cend() const noexcept { return data_ + size_; }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(DynamicArray& other) noexcept {
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

    /** @brief Duplica la capacidad (o asigna 1 slot si actualmente está vacío). */
    void grow() {
        // TODO(estudiante): asignar un nuevo buffer con el doble de la
        // capacidad actual (o 1 si capacity_ es 0), mover los elementos
        // existentes a él, destruir los elementos antiguos, liberar el
        // buffer antiguo y actualizar data_/capacity_.
        throw std::logic_error("DynamicArray::grow: no implementado");
    }

    template <typename U>
    void emplace_back_impl(U&& value) {
        (void)value;
        // TODO(estudiante): crecer el buffer si está lleno, luego
        // construir `value` en el lugar en data_[size_] e incrementar
        // size_.
        throw std::logic_error("DynamicArray::push_back: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_DYNAMIC_ARRAY_HPP
