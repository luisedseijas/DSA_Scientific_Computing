#ifndef DSA_TREES_BINARY_HEAP_HPP
#define DSA_TREES_BINARY_HEAP_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Binary heap: a complete binary tree stored implicitly in a
 * contiguous array, kept ordered by `Compare`.
 *
 * The tree is "complete" (every level full except possibly the last, which
 * fills left to right), which is exactly what lets it be represented
 * without pointers: for a node at index `i`, its parent lives at
 * `(i - 1) / 2` and its children at `2*i + 1` and `2*i + 2`. See
 * docs/guides/05_heaps.md for the full derivation and complexity analysis.
 *
 * With the default `std::less<T>`, `Compare(child, parent)` being false
 * for every node gives a *max-heap* (the convention `std::priority_queue`
 * also uses): the element at index 0 is always the largest according to
 * `Compare`. Passing `std::greater<T>` flips this into a min-heap. In
 * general the root is the element no other element is "less" than under
 * `Compare` — i.e. the maximum of the order `Compare` defines.
 *
 * Storage is a raw, heap-allocated buffer managed by hand (`new`/`delete`
 * at the byte level, placement-new for construction) instead of
 * std::vector, in the same spirit as DynamicArray: the point is to make
 * the underlying array mechanics visible to students, not to reimplement
 * std::vector.
 *
 * @tparam T Element type. Must be move or copy constructible.
 * @tparam Compare Strict-weak-ordering functor; `Compare(a, b)` true means
 * "a should end up below b in the heap". Defaults to std::less<T>, giving
 * a max-heap.
 */
template <typename T, typename Compare = std::less<T>>
class BinaryHeap : public Collection<T> {
   public:
    /** @brief Constructs an empty heap with no allocated storage. */
    BinaryHeap() noexcept : data_(nullptr), size_(0), capacity_(0), compare_() {}

    /** @brief Constructs an empty heap with a custom comparator instance. */
    explicit BinaryHeap(const Compare& compare)
        : data_(nullptr), size_(0), capacity_(0), compare_(compare) {}

    /**
     * @brief Builds a heap from the range `[first, last)` in O(n).
     *
     * This is Floyd's build-heap algorithm: copy the elements in as-is,
     * then sift down every internal node starting from the last one and
     * walking back to the root. It is faster than inserting the elements
     * one at a time (which would cost O(n log n)) because most nodes in a
     * complete tree are near the bottom, where a sift-down does almost no
     * work; the O(n) bound is proven in docs/guides/05_heaps.md via a
     * geometric-series argument.
     *
     * @tparam InputIt Input iterator type.
     * @param first Iterator to the first element.
     * @param last Iterator one past the last element.
     * @param compare Comparator instance to use.
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

    /** @brief Copy constructor. Performs a deep copy of `other`'s elements. */
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

    /** @brief Move constructor. Steals `other`'s buffer, leaving it empty. */
    BinaryHeap(BinaryHeap&& other) noexcept
        : data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_),
          compare_(std::move(other.compare_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    /** @brief Copy assignment operator (copy-and-swap). */
    BinaryHeap& operator=(const BinaryHeap& other) {
        if (this != &other) {
            BinaryHeap tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move assignment operator. */
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

    /** @brief Destroys all elements and releases the underlying storage. */
    ~BinaryHeap() override {
        destroy_all();
        deallocate(data_);
    }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Whether the heap contains no elements. */
    bool empty() const noexcept override { return size_ == 0; }

    /** @brief Destroys every element, leaving size() == 0. Capacity is kept. */
    void clear() override {
        destroy_all();
        size_ = 0;
    }

    /**
     * @brief Inserts `value`, growing the buffer if necessary, and restores
     * the heap invariant. O(log n) amortized.
     */
    void insert(const T& value) { insert_impl(value); }

    /** @brief Inserts `value` via move. O(log n) amortized. */
    void insert(T&& value) { insert_impl(std::move(value)); }

    /** @brief Alias for insert(), matching the classic priority-queue API. */
    void push(const T& value) { insert(value); }
    /** @brief Alias for insert(), matching the classic priority-queue API. */
    void push(T&& value) { insert(std::move(value)); }

    /**
     * @brief Removes and returns the top element (the max under `Compare`,
     * or the min if `Compare` is std::greater<T>). O(log n).
     * @throws std::out_of_range if the heap is empty.
     */
    T extract_top() {
        if (empty()) {
            throw std::out_of_range("BinaryHeap::extract_top: heap is empty");
        }
        T top = std::move(data_[0]);
        --size_;
        if (size_ > 0) {
            // Move the last element to the root, then let it sink to its
            // rightful place. This keeps the tree complete (we only ever
            // remove the very last slot) while restoring the heap order.
            data_[0] = std::move(data_[size_]);
        }
        data_[size_].~T();
        if (size_ > 0) {
            sift_down(0);
        }
        return top;
    }

    /** @brief Alias for extract_top(), matching the classic priority-queue API. */
    T pop() { return extract_top(); }

    /**
     * @brief Read-only access to the top element, without removing it.
     * @throws std::out_of_range if the heap is empty.
     */
    const T& top() const {
        if (empty()) {
            throw std::out_of_range("BinaryHeap::top: heap is empty");
        }
        return data_[0];
    }

    /** @brief Alias for top(), matching the classic priority-queue API. */
    const T& peek() const { return top(); }

    /** @brief Number of elements the current buffer can hold without growing. */
    std::size_t capacity() const noexcept { return capacity_; }

    /** @brief Swaps contents with `other` in O(1). */
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
    void insert_impl(U&& value) {
        if (size_ == capacity_) {
            grow();
        }
        new (data_ + size_) T(std::forward<U>(value));
        ++size_;
        sift_up(size_ - 1);
    }

    /** @brief Index of the parent of node `i`. Undefined if i == 0. */
    static std::size_t parent_of(std::size_t i) { return (i - 1) / 2; }

    /** @brief Index of the left child of node `i`. */
    static std::size_t left_of(std::size_t i) { return 2 * i + 1; }

    /** @brief Index of the right child of node `i`. */
    static std::size_t right_of(std::size_t i) { return 2 * i + 2; }

    /**
     * @brief Moves the element at index `i` up toward the root while it is
     * ordered above its parent under `Compare`, restoring the heap
     * invariant after an insertion at the end of the array.
     *
     * At most one root-to-node path is walked, and the tree has height
     * O(log n), so this is O(log n) in the worst case.
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
     * @brief Moves the element at index `i` down toward the leaves,
     * repeatedly swapping with the child that should be above it under
     * `Compare`, restoring the heap invariant after the root is replaced.
     *
     * Same O(log n) bound as sift_up, for the same reason: at most one
     * root-to-leaf path is walked.
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
     * @brief Floyd's heapify: sifts down every internal node, from the last
     * one to the root, turning an arbitrary array into a valid heap in
     * O(n) total. See docs/guides/05_heaps.md for why this beats the naive
     * O(n log n) of inserting elements one at a time.
     */
    void build_heap() {
        if (size_ < 2) {
            return;
        }
        // The last internal (non-leaf) node is the parent of the last
        // element; everything after it is a leaf and trivially a valid
        // (single-node) heap already.
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
