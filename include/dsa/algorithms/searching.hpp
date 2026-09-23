#ifndef DSA_ALGORITHMS_SEARCHING_HPP
#define DSA_ALGORITHMS_SEARCHING_HPP

#include <iterator>

namespace dsa {

namespace detail {

/**
 * @brief Recursive binary search helper that keeps track of the original
 * end iterator (`sentinel`) separately from the current subrange's `last`,
 * since narrowing the search to the left half shrinks `last` but must still
 * report "not found" as the caller's original end iterator.
 */
template <typename RandomIt, typename T>
RandomIt binary_search_recursive_impl(RandomIt first, RandomIt last, RandomIt sentinel,
                                      const T& value) {
    auto count = std::distance(first, last);
    if (count <= 0) return sentinel;

    RandomIt mid = first;
    std::advance(mid, count / 2);

    if (*mid < value) {
        return binary_search_recursive_impl(mid + 1, last, sentinel, value);
    }
    if (value < *mid) {
        return binary_search_recursive_impl(first, mid, sentinel, value);
    }
    return mid;
}

}  // namespace detail

/**
 * @brief Searches [first, last) for the first element equal to `value`.
 *
 * Works on any input iterator; makes no assumptions about ordering.
 *
 * @tparam InputIt Input iterator type.
 * @tparam T Value type, compared with operator==.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param value Value to search for.
 * @return Iterator to the first matching element, or `last` if not found.
 *
 * Complexity: O(n) comparisons in the worst case; O(1) best case.
 */
template <typename InputIt, typename T>
InputIt linear_search(InputIt first, InputIt last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) return first;
    }
    return last;
}

/**
 * @brief Searches the sorted range [first, last) for `value` using iterative
 * binary search.
 *
 * Precondition: [first, last) must be sorted in ascending order according
 * to operator< (the same order std::sort/dsa sorting functions produce by
 * default). Passing an unsorted range yields undefined/incorrect results.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam T Value type, compared with operator<.
 * @param first Iterator to the beginning of the sorted range.
 * @param last Iterator to one-past-the-end of the sorted range.
 * @param value Value to search for.
 * @return Iterator to a matching element, or `last` if not found.
 *
 * Complexity: O(log n) comparisons.
 */
template <typename RandomIt, typename T>
RandomIt binary_search(RandomIt first, RandomIt last, const T& value) {
    auto count = std::distance(first, last);
    while (count > 0) {
        auto step = count / 2;
        RandomIt mid = first;
        std::advance(mid, step);
        if (*mid < value) {
            first = ++mid;
            count -= step + 1;
        } else if (value < *mid) {
            count = step;
        } else {
            return mid;
        }
    }
    return last;
}

/**
 * @brief Recursive variant of binary search, provided as a pedagogical
 * counterpart to the iterative dsa::binary_search: same O(log n) complexity
 * and precondition (sorted range), but relies on the call stack (O(log n)
 * extra space) instead of a loop, which is useful for comparing the two
 * styles.
 *
 * Precondition: [first, last) must be sorted in ascending order.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam T Value type, compared with operator<.
 * @param first Iterator to the beginning of the sorted range.
 * @param last Iterator to one-past-the-end of the sorted range.
 * @param value Value to search for.
 * @return Iterator to a matching element, or `last` if not found.
 */
template <typename RandomIt, typename T>
RandomIt binary_search_recursive(RandomIt first, RandomIt last, const T& value) {
    return detail::binary_search_recursive_impl(first, last, last, value);
}

}  // namespace dsa

#endif  // DSA_ALGORITHMS_SEARCHING_HPP
