#ifndef DSA_LINEAR_QUEUE_HPP
#define DSA_LINEAR_QUEUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A FIFO (first-in, first-out) queue backed by a circular buffer.
 *
 * Queue owns a raw, heap-allocated buffer of capacity `capacity_`. Elements
 * live in the slots `[front_, front_ + size_)`, indices taken modulo
 * `capacity_` ("wrapping around" the end of the buffer back to index 0).
 * Tracking a `front_` index instead of always keeping the first element at
 * index 0 is what makes dequeue() O(1): a naive array-backed queue that
 * shifts every remaining element left after a dequeue is O(n). See
 * docs/guides/03_stacks_queues.md for the full argument and a Big-O table.
 *
 * When the buffer fills up, a new, larger buffer is allocated and the
 * existing elements are copied/moved into it starting at index 0 (the
 * wraparound is "unrolled" during the copy), exactly as with a dynamic
 * array's growth. This gives enqueue() amortized O(1) cost.
 *
 * The class manages memory manually with placement-new/explicit destructor
 * calls instead of wrapping std::vector or std::deque, so students can see
 * exactly how a circular buffer works under the hood.
 *
 * @tparam T Element type. Must be move or copy constructible.
 */
template <typename T>
class Queue : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Constructs an empty Queue with no allocated storage. */
    Queue() noexcept : data_(nullptr), front_(0), size_(0), capacity_(0) {}

    /** @brief Copy constructor. Performs a deep copy of `other`'s elements. */
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

    /** @brief Move constructor. Steals `other`'s buffer, leaving it empty. */
    Queue(Queue&& other) noexcept
        : data_(other.data_), front_(other.front_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.front_ = 0;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Copy assignment operator (copy-and-swap). */
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            Queue tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move assignment operator. */
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

    /** @brief Destroys all elements and releases the underlying storage. */
    ~Queue() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Number of elements the current buffer can hold without growing. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Whether the queue contains no elements. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destroys every element, leaving size() == 0. Capacity is kept. */
    void clear() override {
        destroy_all();
        size_ = 0;
        front_ = 0;
    }

    /** @brief Inserts `value` at the back of the queue. Amortized O(1). */
    void enqueue(const T& value) { enqueue_impl(value); }

    /** @brief Inserts `value` at the back of the queue via move. Amortized O(1). */
    void enqueue(T&& value) { enqueue_impl(std::move(value)); }

    /**
     * @brief Removes the element at the front of the queue. O(1).
     * @throws std::out_of_range if the queue is empty.
     */
    void dequeue() {
        // TODO(estudiante): destruir el elemento en front_, avanzar front_
        // una posición (con wraparound) y decrementar size_.
        throw std::logic_error("Queue::dequeue: no implementado");
    }

    /**
     * @brief Reference to the element at the front of the queue.
     * @throws std::out_of_range if the queue is empty.
     */
    reference front() {
        if (empty()) {
            throw std::out_of_range("Queue::front: queue is empty");
        }
        return data_[front_];
    }

    /** @brief Const reference to the element at the front of the queue. */
    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("Queue::front: queue is empty");
        }
        return data_[front_];
    }

    /**
     * @brief Reference to the element at the back of the queue.
     * @throws std::out_of_range if the queue is empty.
     */
    reference back() {
        if (empty()) {
            throw std::out_of_range("Queue::back: queue is empty");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Const reference to the element at the back of the queue. */
    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("Queue::back: queue is empty");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Swaps contents with `other` in O(1). */
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

    /** @brief Physical buffer index of the logical position `offset` from front_. */
    std::size_t index_of(std::size_t offset) const { return advance(front_, offset); }

    /** @brief Adds `steps` to `index`, wrapping around `capacity_`. */
    std::size_t advance(std::size_t index, std::size_t steps) const {
        return (index + steps) % capacity_;
    }

    void destroy_all() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[index_of(i)].~T();
        }
    }

    /** @brief Doubles capacity (or allocates 1 slot if currently empty), unrolling wraparound. */
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
