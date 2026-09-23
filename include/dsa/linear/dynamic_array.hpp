#ifndef DSA_LINEAR_DYNAMIC_ARRAY_HPP
#define DSA_LINEAR_DYNAMIC_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A dynamic array (resizable, contiguous-storage array), similar in
 * spirit to std::vector.
 *
 * DynamicArray owns a raw, heap-allocated buffer of capacity `capacity_`
 * that stores `size_` constructed elements at the front. When the buffer
 * fills up, a new buffer with double the capacity is allocated and the
 * existing elements are moved (or copied) into it. This geometric growth
 * strategy is what gives push_back its amortized O(1) cost: see
 * docs/guides/01_dynamic_array.md for the full amortized-analysis argument.
 *
 * The class intentionally manages memory manually with `new`/`delete`
 * instead of wrapping a std::vector, so that students can see exactly how
 * a dynamic array is implemented under the hood (allocation, placement of
 * elements, growth, and the Rule of Five).
 *
 * @tparam T Element type. Must be move or copy constructible.
 */
template <typename T>
class DynamicArray : public Collection<T> {
   public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;

    /** @brief Constructs an empty DynamicArray with no allocated storage. */
    DynamicArray() noexcept : data_(nullptr), size_(0), capacity_(0) {}

    /**
     * @brief Constructs an empty DynamicArray that has already reserved
     * storage for `initial_capacity` elements.
     * @param initial_capacity Number of elements worth of storage to
     * pre-allocate.
     */
    explicit DynamicArray(std::size_t initial_capacity) : data_(nullptr), size_(0), capacity_(0) {
        if (initial_capacity > 0) {
            data_ = allocate(initial_capacity);
            capacity_ = initial_capacity;
        }
    }

    /** @brief Copy constructor. Performs a deep copy of `other`'s elements. */
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

    /** @brief Move constructor. Steals `other`'s buffer, leaving it empty. */
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Copy assignment operator (copy-and-swap). */
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move assignment operator. */
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

    /** @brief Destroys all elements and releases the underlying storage. */
    ~DynamicArray() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Number of elements the current buffer can hold without growing. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Whether the array contains no elements. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destroys every element, leaving size() == 0. Capacity is kept. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /**
     * @brief Appends `value` at the end, growing the buffer if necessary.
     * Amortized O(1).
     */
    void push_back(const T& value) { emplace_back_impl(value); }

    /** @brief Appends `value` at the end via move. Amortized O(1). */
    void push_back(T&& value) { emplace_back_impl(std::move(value)); }

    /**
     * @brief Removes the last element.
     * @throws std::out_of_range if the array is empty.
     */
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("DynamicArray::pop_back: array is empty");
        }
        --size_;
        data_[size_].~T();
    }

    /**
     * @brief Inserts `value` at `index`, shifting subsequent elements right.
     * O(n) in the worst case.
     * @param index Position at which to insert; must satisfy 0 <= index <=
     * size().
     * @throws std::out_of_range if index > size().
     */
    void insert(std::size_t index, const T& value) {
        if (index > size_) {
            throw std::out_of_range("DynamicArray::insert: index out of range");
        }
        if (size_ == capacity_) {
            grow();
        }
        if (index == size_) {
            // Fast path: inserting at the end is just an append.
            new (data_ + size_) T(value);
        } else {
            // Move the current last element into the new, uninitialized slot,
            // then shift [index, size_ - 1) right by one via move-assignment,
            // back to front so no live value is overwritten before it is read.
            new (data_ + size_) T(std::move(data_[size_ - 1]));
            for (std::size_t i = size_ - 1; i > index; --i) {
                data_[i] = std::move(data_[i - 1]);
            }
            data_[index] = value;
        }
        ++size_;
    }

    /**
     * @brief Removes the element at `index`, shifting subsequent elements
     * left. O(n) in the worst case.
     * @throws std::out_of_range if index >= size().
     */
    void erase(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray::erase: index out of range");
        }
        for (std::size_t i = index; i + 1 < size_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        data_[size_].~T();
    }

    /** @brief Unchecked element access. Behavior is undefined if out of range. */
    reference operator[](std::size_t index) { return data_[index]; }

    /** @brief Unchecked const element access. */
    const_reference operator[](std::size_t index) const { return data_[index]; }

    /**
     * @brief Bounds-checked element access.
     * @throws std::out_of_range if index >= size().
     */
    reference at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray::at: index out of range");
        }
        return data_[index];
    }

    /** @brief Bounds-checked const element access. */
    const_reference at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray::at: index out of range");
        }
        return data_[index];
    }

    /** @brief Reference to the first element. Undefined behavior if empty. */
    reference front() { return data_[0]; }
    /** @brief Const reference to the first element. */
    const_reference front() const { return data_[0]; }

    /** @brief Reference to the last element. Undefined behavior if empty. */
    reference back() { return data_[size_ - 1]; }
    /** @brief Const reference to the last element. */
    const_reference back() const { return data_[size_ - 1]; }

    /** @brief Pointer to the underlying contiguous storage. */
    T* data() noexcept { return data_; }
    /** @brief Const pointer to the underlying contiguous storage. */
    const T* data() const noexcept { return data_; }

    // -- Iterators, enabling range-for and <algorithm> compatibility. --------

    iterator begin() noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator end() const noexcept { return data_ + size_; }
    const_iterator cbegin() const noexcept { return data_; }
    const_iterator cend() const noexcept { return data_ + size_; }

    /** @brief Swaps contents with `other` in O(1). */
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
    void emplace_back_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        new (data_ + size_) T(std::forward<U>(value));
        ++size_;
    }
};

}  // namespace dsa

#endif  // DSA_LINEAR_DYNAMIC_ARRAY_HPP
