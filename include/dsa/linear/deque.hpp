#ifndef DSA_LINEAR_DEQUE_HPP
#define DSA_LINEAR_DEQUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A double-ended queue backed by a circular buffer.
 *
 * Deque owns a raw, heap-allocated buffer of capacity `capacity_`. Elements
 * live in the slots `[front_, front_ + size_)`, indices taken modulo
 * `capacity_` (the same wraparound scheme as Queue). Keeping a `front_`
 * index that can move both forward and backward around the buffer is what
 * lets push_front()/pop_front() and push_back()/pop_back() all run in O(1):
 * see docs/guides/03_stacks_queues.md for the full argument.
 *
 * When the buffer fills up, a new, larger buffer is allocated and the
 * existing elements are copied/moved into it starting at index 0, exactly
 * as with Queue's growth. This gives both push_front() and push_back()
 * amortized O(1) cost.
 *
 * The class manages memory manually with placement-new/explicit destructor
 * calls instead of wrapping std::deque, so students can see exactly how a
 * double-ended circular buffer works under the hood.
 *
 * @tparam T Element type. Must be move or copy constructible.
 */
template <typename T>
class Deque : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    /** @brief Constructs an empty Deque with no allocated storage. */
    Deque() noexcept : data_(nullptr), front_(0), size_(0), capacity_(0) {}

    /** @brief Copy constructor. Performs a deep copy of `other`'s elements. */
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

    /** @brief Move constructor. Steals `other`'s buffer, leaving it empty. */
    Deque(Deque&& other) noexcept
        : data_(other.data_), front_(other.front_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.front_ = 0;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Copy assignment operator (copy-and-swap). */
    Deque& operator=(const Deque& other) {
        if (this != &other) {
            Deque tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move assignment operator. */
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

    /** @brief Destroys all elements and releases the underlying storage. */
    ~Deque() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Number of elements the current buffer can hold without growing. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Whether the deque contains no elements. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destroys every element, leaving size() == 0. Capacity is kept. */
    void clear() override {
        destroy_all();
        size_ = 0;
        front_ = 0;
    }

    /** @brief Inserts `value` at the front of the deque. Amortized O(1). */
    void push_front(const T& value) { push_front_impl(value); }

    /** @brief Inserts `value` at the front of the deque via move. Amortized O(1). */
    void push_front(T&& value) { push_front_impl(std::move(value)); }

    /** @brief Inserts `value` at the back of the deque. Amortized O(1). */
    void push_back(const T& value) { push_back_impl(value); }

    /** @brief Inserts `value` at the back of the deque via move. Amortized O(1). */
    void push_back(T&& value) { push_back_impl(std::move(value)); }

    /**
     * @brief Removes the element at the front of the deque. O(1).
     * @throws std::out_of_range if the deque is empty.
     */
    void pop_front() {
        if (empty()) {
            throw std::out_of_range("Deque::pop_front: deque is empty");
        }
        data_[front_].~T();
        front_ = advance(front_, 1);
        --size_;
    }

    /**
     * @brief Removes the element at the back of the deque. O(1).
     * @throws std::out_of_range if the deque is empty.
     */
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("Deque::pop_back: deque is empty");
        }
        data_[index_of(size_ - 1)].~T();
        --size_;
    }

    /**
     * @brief Reference to the element at the front of the deque.
     * @throws std::out_of_range if the deque is empty.
     */
    reference front() {
        if (empty()) {
            throw std::out_of_range("Deque::front: deque is empty");
        }
        return data_[front_];
    }

    /** @brief Const reference to the element at the front of the deque. */
    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("Deque::front: deque is empty");
        }
        return data_[front_];
    }

    /**
     * @brief Reference to the element at the back of the deque.
     * @throws std::out_of_range if the deque is empty.
     */
    reference back() {
        if (empty()) {
            throw std::out_of_range("Deque::back: deque is empty");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Const reference to the element at the back of the deque. */
    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("Deque::back: deque is empty");
        }
        return data_[index_of(size_ - 1)];
    }

    /** @brief Swaps contents with `other` in O(1). */
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

    /** @brief Physical buffer index of the logical position `offset` from front_. */
    std::size_t index_of(std::size_t offset) const { return advance(front_, offset); }

    /** @brief Adds `steps` to `index`, wrapping around `capacity_`. */
    std::size_t advance(std::size_t index, std::size_t steps) const {
        return (index + steps) % capacity_;
    }

    /** @brief Subtracts one step from `index`, wrapping around `capacity_`. */
    std::size_t retreat(std::size_t index) const {
        return (index == 0) ? capacity_ - 1 : index - 1;
    }

    void destroy_all() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[index_of(i)].~T();
        }
    }

    /** @brief Doubles capacity (or allocates 1 slot if currently empty), unrolling wraparound. */
    void grow() {
        std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        T* new_data = allocate(new_capacity);
        for (std::size_t i = 0; i < size_; ++i) {
            std::size_t src = index_of(i);
            new (new_data + i) T(std::move(data_[src]));
            data_[src].~T();
        }
        deallocate(data_);
        data_ = new_data;
        capacity_ = new_capacity;
        front_ = 0;
    }

    template <typename U>
    void push_front_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        front_ = retreat(front_);
        new (data_ + front_) T(std::forward<U>(value));
        ++size_;
    }

    template <typename U>
    void push_back_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        std::size_t back_index = advance(front_, size_);
        new (data_ + back_index) T(std::forward<U>(value));
        ++size_;
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_DEQUE_HPP
