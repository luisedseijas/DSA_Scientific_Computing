#ifndef DSA_LINEAR_STACK_HPP
#define DSA_LINEAR_STACK_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A LIFO (last-in, first-out) stack backed by its own dynamic array.
 *
 * Stack owns a raw, heap-allocated buffer of capacity `capacity_` that
 * stores `size_` constructed elements at the front, exactly like
 * DynamicArray. Growth is geometric (capacity doubles when full), which
 * gives push() amortized O(1) cost. See docs/guides/03_stacks_queues.md
 * for the full theory (LIFO discipline, use cases, Big-O table).
 *
 * The class manages memory manually with placement-new/explicit destructor
 * calls instead of wrapping std::vector, so students can see exactly how a
 * stack is implemented on top of a raw buffer (allocation, growth, Rule of
 * Five).
 *
 * @tparam T Element type. Must be move or copy constructible.
 */
template <typename T>
class Stack : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Constructs an empty Stack with no allocated storage. */
    Stack() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    /** @brief Copy constructor. Performs a deep copy of `other`'s elements. */
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

    /** @brief Move constructor. Steals `other`'s buffer, leaving it empty. */
    Stack(Stack&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Copy assignment operator (copy-and-swap). */
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            Stack tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move assignment operator. */
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

    /** @brief Destroys all elements and releases the underlying storage. */
    ~Stack() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Number of elements the current buffer can hold without growing. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Whether the stack contains no elements. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destroys every element, leaving size() == 0. Capacity is kept. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /** @brief Pushes `value` onto the top of the stack. Amortized O(1). */
    void push(const T& value) { push_impl(value); }

    /** @brief Pushes `value` onto the top of the stack via move. Amortized O(1). */
    void push(T&& value) { push_impl(std::move(value)); }

    /**
     * @brief Removes the element at the top of the stack. O(1).
     * @throws std::out_of_range if the stack is empty.
     */
    void pop() {
        if (empty()) {
            throw std::out_of_range("Stack::pop: stack is empty");
        }
        --size_;
        data_[size_].~T();
    }

    /**
     * @brief Reference to the element at the top of the stack.
     * @throws std::out_of_range if the stack is empty.
     */
    reference top() {
        if (empty()) {
            throw std::out_of_range("Stack::top: stack is empty");
        }
        return data_[size_ - 1];
    }

    /** @brief Const reference to the element at the top of the stack. */
    const_reference top() const {
        if (empty()) {
            throw std::out_of_range("Stack::top: stack is empty");
        }
        return data_[size_ - 1];
    }

    /** @brief Swaps contents with `other` in O(1). */
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

    /** @brief Doubles capacity (or allocates 1 slot if currently empty). */
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
    void push_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        new (data_ + size_) T(std::forward<U>(value));
        ++size_;
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_STACK_HPP
