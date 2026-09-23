#ifndef DSA_TREES_BINARY_HEAP_HPP
#define DSA_TREES_BINARY_HEAP_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Montículo binario (binary heap): un árbol binario completo
 * almacenado implícitamente en un arreglo contiguo, mantenido ordenado
 * según `Compare`.
 *
 * El árbol es "completo" (cada nivel está lleno excepto posiblemente el
 * último, que se llena de izquierda a derecha), lo cual es precisamente lo
 * que permite representarlo sin punteros: para un nodo en el índice `i`,
 * su padre está en `(i - 1) / 2` y sus hijos en `2*i + 1` y `2*i + 2`. Ver
 * docs/guides/05_heaps.md para la derivación completa y el análisis de
 * complejidad.
 *
 * Con el `std::less<T>` por defecto, que `Compare(hijo, padre)` sea falso
 * para cada nodo da como resultado un *max-heap* (la convención que
 * también usa `std::priority_queue`): el elemento en el índice 0 es
 * siempre el mayor según `Compare`. Pasar `std::greater<T>` invierte esto
 * a un min-heap. En general, la raíz es el elemento que ningún otro
 * elemento supera bajo `Compare` — es decir, el máximo del orden que
 * `Compare` define.
 *
 * El almacenamiento es un buffer crudo reservado en el heap y gestionado
 * manualmente (`new`/`delete` a nivel de bytes, placement-new para la
 * construcción) en lugar de std::vector, en el mismo espíritu que
 * DynamicArray: el objetivo es hacer visible a los estudiantes la mecánica
 * subyacente del arreglo, no reimplementar std::vector.
 *
 * @tparam T Tipo del elemento. Debe ser move o copy constructible.
 * @tparam Compare Functor de orden estricto débil; `Compare(a, b)`
 * verdadero significa "a debería terminar por debajo de b en el
 * montículo". Por defecto std::less<T>, dando un max-heap.
 */
template <typename T, typename Compare = std::less<T>>
class BinaryHeap : public Collection<T> {
   public:
    /** @brief Construye un montículo vacío sin almacenamiento reservado. */
    BinaryHeap() noexcept : data_(nullptr), size_(0), capacity_(0), compare_() {}

    /** @brief Construye un montículo vacío con una instancia de comparador personalizada. */
    explicit BinaryHeap(const Compare& compare)
        : data_(nullptr), size_(0), capacity_(0), compare_(compare) {}

    /**
     * @brief Construye un montículo a partir del rango `[first, last)` en O(n).
     *
     * Este es el algoritmo de construcción de montículos de Floyd: se
     * copian los elementos tal cual, y luego se hunde (sift down) cada
     * nodo interno empezando por el último y retrocediendo hasta la raíz.
     * Es más rápido que insertar los elementos uno a uno (lo cual costaría
     * O(n log n)) porque la mayoría de los nodos de un árbol completo
     * están cerca de la parte inferior, donde un sift-down casi no hace
     * trabajo; la cota O(n) se demuestra en docs/guides/05_heaps.md
     * mediante un argumento de serie geométrica.
     *
     * @tparam InputIt Tipo de iterador de entrada.
     * @param first Iterador al primer elemento.
     * @param last Iterador una posición después del último elemento.
     * @param compare Instancia de comparador a usar.
     */
    template <typename InputIt>
    BinaryHeap(InputIt first, InputIt last, const Compare& compare = Compare())
        : data_(nullptr), size_(0), capacity_(0), compare_(compare) {
        for (InputIt it = first; it != last; ++it) {
            if (size_ == capacity_) {
                grow();
            }
            new (data_ + size_) T(*it);
            ++size_;
        }
        build_heap();
    }

    /** @brief Constructor de copia. Realiza una copia profunda de los elementos de `other`. */
    BinaryHeap(const BinaryHeap& other)
        : data_(nullptr), size_(0), capacity_(0), compare_(other.compare_) {
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
    BinaryHeap(BinaryHeap&& other) noexcept
        : data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_),
          compare_(std::move(other.compare_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Operador de asignación por copia (copy-and-swap). */
    BinaryHeap& operator=(const BinaryHeap& other) {
        if (this != &other) {
            BinaryHeap tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Operador de asignación por movimiento. */
    BinaryHeap& operator=(BinaryHeap&& other) noexcept {
        if (this != &other) {
            destroy_all();
            deallocate(data_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            compare_ = std::move(other.compare_);
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    /** @brief Destruye todos los elementos y libera el almacenamiento subyacente. */
    ~BinaryHeap() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Número de elementos almacenados actualmente. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Indica si el montículo no contiene elementos. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destruye cada elemento, dejando size() == 0. Se conserva la capacidad. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /**
     * @brief Inserta `value`, agrandando el buffer si es necesario, y
     * restaura el invariante del montículo. O(log n) amortizado.
     */
    void insert(const T& value) { insert_impl(value); }

    /** @brief Inserta `value` mediante move. O(log n) amortizado. */
    void insert(T&& value) { insert_impl(std::move(value)); }

    /** @brief Alias de insert(), igualando la API clásica de cola de prioridad. */
    void push(const T& value) { insert(value); }
    /** @brief Alias de insert(), igualando la API clásica de cola de prioridad. */
    void push(T&& value) { insert(std::move(value)); }

    /**
     * @brief Elimina y devuelve el elemento superior (el máximo según
     * `Compare`, o el mínimo si `Compare` es std::greater<T>). O(log n).
     * @throws std::out_of_range si el montículo está vacío.
     */
    T extract_top() {
        if (empty()) {
            throw std::out_of_range("BinaryHeap::extract_top: el montículo está vacío");
        }
        T top = std::move(data_[0]);
        --size_;
        if (size_ > 0) {
            // Mueve el último elemento a la raíz y luego lo deja hundirse
            // hasta su lugar correspondiente. Esto mantiene el árbol
            // completo (solo se elimina la última posición) mientras se
            // restaura el orden del montículo.
            data_[0] = std::move(data_[size_]);
        }
        data_[size_].~T();
        if (size_ > 0) {
            sift_down(0);
        }
        return top;
    }

    /** @brief Alias de extract_top(), igualando la API clásica de cola de prioridad. */
    T pop() { return extract_top(); }

    /**
     * @brief Acceso de solo lectura al elemento superior, sin eliminarlo.
     * @throws std::out_of_range si el montículo está vacío.
     */
    const T& top() const {
        if (empty()) {
            throw std::out_of_range("BinaryHeap::top: el montículo está vacío");
        }
        return data_[0];
    }

    /** @brief Alias de top(), igualando la API clásica de cola de prioridad. */
    const T& peek() const { return top(); }

    /** @brief Número de elementos que el buffer actual puede contener sin crecer. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Intercambia el contenido con `other` en O(1). */
    void swap(BinaryHeap& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(compare_, other.compare_);
    }

   private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Compare compare_;

    static T* allocate(std::size_t n) { return static_cast<T*>(::operator new(n * sizeof(T))); }

    static void deallocate(T* ptr) { ::operator delete(ptr); }

    void destroy_all() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
    }

    /** @brief Duplica la capacidad (o reserva 1 posición si está vacío). */
    void grow() {
        std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        T* new_data = allocate(new_capacity);
        for (std::size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        deallocate(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    template <typename U>
    void insert_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        new (data_ + size_) T(std::forward<U>(value));
        ++size_;
        sift_up(size_ - 1);
    }

    /** @brief Índice del padre del nodo `i`. Indefinido si i == 0. */
    static std::size_t parent_of(std::size_t i) { return (i - 1) / 2; }

    /** @brief Índice del hijo izquierdo del nodo `i`. */
    static std::size_t left_of(std::size_t i) { return 2 * i + 1; }

    /** @brief Índice del hijo derecho del nodo `i`. */
    static std::size_t right_of(std::size_t i) { return 2 * i + 2; }

    /**
     * @brief Mueve el elemento en el índice `i` hacia arriba, hacia la
     * raíz, mientras esté ordenado por encima de su padre según
     * `Compare`, restaurando el invariante del montículo tras una
     * inserción al final del arreglo.
     *
     * A lo sumo se recorre un camino de la raíz a un nodo, y el árbol
     * tiene altura O(log n), así que esto es O(log n) en el peor caso.
     */
    void sift_up(std::size_t i) {
        (void)i;
        // TODO(estudiante): mover el elemento en el índice i hacia la raíz
        // mientras esté ordenado por encima de su padre según Compare,
        // intercambiando en cada paso, hasta llegar a la raíz o encontrar
        // un padre que no deba ceder su lugar.
        throw std::logic_error("BinaryHeap::sift_up: no implementado");
    }

    /**
     * @brief Mueve el elemento en el índice `i` hacia abajo, hacia las
     * hojas, intercambiándolo repetidamente con el hijo que deba quedar
     * por encima de él según `Compare`, restaurando el invariante del
     * montículo después de reemplazar la raíz.
     *
     * Misma cota O(log n) que sift_up, por la misma razón: a lo sumo se
     * recorre un camino de la raíz a una hoja.
     */
    void sift_down(std::size_t i) {
        (void)i;
        // TODO(estudiante): mover el elemento en el índice i hacia las hojas,
        // intercambiando repetidamente con el hijo que deba quedar por
        // encima de él según Compare, hasta que ningún hijo lo supere o se
        // llegue a una hoja.
        throw std::logic_error("BinaryHeap::sift_down: no implementado");
    }

    /**
     * @brief Heapify de Floyd: hunde (sift down) cada nodo interno, desde
     * el último hasta la raíz, convirtiendo un arreglo arbitrario en un
     * montículo válido en O(n) total. Ver docs/guides/05_heaps.md para
     * entender por qué esto supera al enfoque ingenuo de O(n log n) de
     * insertar los elementos uno a uno.
     */
    void build_heap() {
        if (size_ < 2) {
            return;
        }
        // El último nodo interno (no hoja) es el padre del último
        // elemento; todo lo que viene después es una hoja y ya
        // trivialmente es un montículo válido (de un solo nodo).
        std::size_t i = parent_of(size_ - 1);
        while (true) {
            sift_down(i);
            if (i == 0) {
                break;
            }
            --i;
        }
    }
};

}  // namespace dsa

#endif  // DSA_TREES_BINARY_HEAP_HPP
