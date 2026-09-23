#ifndef DSA_TREES_PRIORITY_QUEUE_HPP
#define DSA_TREES_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include <utility>

#include "dsa/interfaces/collection.hpp"
#include "dsa/trees/binary_heap.hpp"

namespace dsa {

/**
 * @brief Priority queue ADT: always serves its "highest priority" element
 * first, where priority is defined by `Compare`.
 *
 * This class is intentionally a thin adapter, not a second implementation:
 * a priority queue *is* a binary heap under a different name and a
 * narrower API (compare this to how std::priority_queue in the standard
 * library adapts an underlying container, typically std::vector, in
 * exactly the same way). All the real work - array storage, sift_up,
 * sift_down, O(n) build_heap - lives in BinaryHeap; see
 * include/dsa/trees/binary_heap.hpp and docs/guides/05_heaps.md.
 *
 * With the default std::less<T>, higher-valued elements have higher
 * priority (a max-priority queue). Pass std::greater<T> for a
 * min-priority queue, exactly as with BinaryHeap and std::priority_queue.
 *
 * @tparam T Element type. Must be move or copy constructible.
 * @tparam Compare Strict-weak-ordering functor defining priority order.
 * Defaults to std::less<T>.
 */
template <typename T, typename Compare = std::less<T>>
class PriorityQueue : public Collection<T> {
   public:
    /** @brief Constructs an empty priority queue. */
    PriorityQueue() = default;

    /** @brief Constructs an empty priority queue with a custom comparator. */
    explicit PriorityQueue(const Compare& compare) : heap_(compare) {}

    /**
     * @brief Builds a priority queue from the range `[first, last)` in
     * O(n), by delegating to BinaryHeap's build_heap constructor.
     */
    template <typename InputIt>
    PriorityQueue(InputIt first, InputIt last, const Compare& compare = Compare())
        : heap_(first, last, compare) {}

    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue(PriorityQueue&&) noexcept = default;
    PriorityQueue& operator=(const PriorityQueue&) = default;
    PriorityQueue& operator=(PriorityQueue&&) noexcept = default;
    ~PriorityQueue() override = default;

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return heap_.size(); }

    /** @brief Whether the priority queue contains no elements. */
    bool empty() const noexcept override { return heap_.empty(); }

    /** @brief Removes every element. */
    void clear() override { heap_.clear(); }

    /** @brief Inserts `value` according to its priority. O(log n) amortized. */
    void push(const T& value) { heap_.push(value); }
    /** @brief Inserts `value` via move. O(log n) amortized. */
    void push(T&& value) { heap_.push(std::move(value)); }

    /**
     * @brief Removes and returns the highest-priority element. O(log n).
     * @throws std::out_of_range if the queue is empty.
     */
    T pop() { return heap_.pop(); }

    /**
     * @brief Read-only access to the highest-priority element.
     * @throws std::out_of_range if the queue is empty.
     */
    const T& top() const { return heap_.top(); }

    /** @brief Swaps contents with `other` in O(1). */
    void swap(PriorityQueue& other) noexcept { heap_.swap(other.heap_); }

   private:
    BinaryHeap<T, Compare> heap_;
};

}  // namespace dsa

#endif  // DSA_TREES_PRIORITY_QUEUE_HPP
