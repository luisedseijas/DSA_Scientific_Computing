#ifndef DSA_ALGORITHMS_SORTING_HPP
#define DSA_ALGORITHMS_SORTING_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace dsa {

/**
 * @brief Sorts the range [first, last) using bubble sort.
 *
 * Repeatedly steps through the range, swapping adjacent elements that are
 * out of order. Stops early once a full pass makes no swaps.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n^2) comparisons/swaps in the worst and average case,
 * O(n) in the best case (already sorted, thanks to the early exit).
 * Stable: yes (only swaps adjacent elements that are strictly out of order).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void bubble_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): recorre repetidamente el rango intercambiando pares
    // adyacentes fuera de orden, hasta que una pasada completa no haga swaps.
    throw std::logic_error("dsa::bubble_sort: no implementado");
}

/**
 * @brief Sorts the range [first, last) using insertion sort.
 *
 * Builds the sorted range incrementally by inserting each element into its
 * correct position among the already-sorted prefix.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n^2) worst/average case, O(n) best case (already sorted).
 * Stable: yes.
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void insertion_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): construye el rango ordenado incrementalmente,
    // insertando cada elemento en su posicion correcta dentro del prefijo ya
    // ordenado.
    throw std::logic_error("dsa::insertion_sort: no implementado");
}

/**
 * @brief Sorts the range [first, last) using selection sort.
 *
 * Repeatedly selects the minimum (per comp) element of the unsorted suffix
 * and swaps it into place.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n^2) in every case (best, average and worst), since the
 * unsorted suffix is always scanned in full regardless of its order.
 * Stable: no (the swap can move an equal element past another equal one).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void selection_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): en cada iteracion, busca el minimo del sufijo no
    // ordenado (segun comp) e intercambialo con el inicio de ese sufijo.
    throw std::logic_error("dsa::selection_sort: no implementado");
}

namespace detail {

/**
 * @brief Merges two adjacent sorted subranges [first, mid) and [mid, last)
 * into a single sorted range using an auxiliary buffer.
 */
template <typename RandomIt, typename Compare>
void merge_ranges(RandomIt first, RandomIt mid, RandomIt last, Compare comp) {
    using ValueType = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<ValueType> buffer;
    buffer.reserve(static_cast<std::size_t>(std::distance(first, last)));

    RandomIt left = first;
    RandomIt right = mid;
    while (left != mid && right != last) {
        if (comp(*right, *left)) {
            buffer.push_back(std::move(*right));
            ++right;
        } else {
            buffer.push_back(std::move(*left));
            ++left;
        }
    }
    while (left != mid) {
        buffer.push_back(std::move(*left));
        ++left;
    }
    while (right != last) {
        buffer.push_back(std::move(*right));
        ++right;
    }
    std::move(buffer.begin(), buffer.end(), first);
}

}  // namespace detail

/**
 * @brief Sorts the range [first, last) using recursive merge sort.
 *
 * Splits the range in half, recursively sorts each half, then merges the
 * two sorted halves using an auxiliary buffer (see detail::merge_ranges).
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n log n) in the best, average and worst case.
 * Space: O(n) auxiliary due to the merge buffer.
 * Stable: yes (the merge step prefers the left subrange on ties).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void merge_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): divide el rango a la mitad, ordena recursivamente
    // cada mitad y fusionalas con detail::merge_ranges.
    throw std::logic_error("dsa::merge_sort: no implementado");
}

namespace detail {

/**
 * @brief Chooses a pivot using the median-of-three strategy (first, middle,
 * last elements) and moves it to (last - 1), Hoare/Lomuto-friendly setup.
 * This avoids O(n^2) worst-case behavior on already-sorted or reverse-sorted
 * input, which a naive "always pick the last element" strategy suffers from.
 */
template <typename RandomIt, typename Compare>
RandomIt median_of_three_pivot(RandomIt first, RandomIt last, Compare comp) {
    RandomIt mid = first + std::distance(first, last) / 2;
    RandomIt end = last - 1;
    if (comp(*mid, *first)) std::iter_swap(mid, first);
    if (comp(*end, *first)) std::iter_swap(end, first);
    if (comp(*end, *mid)) std::iter_swap(end, mid);
    // *mid now holds the median of {first, mid, end}. Move it next to the end
    // so it can be used as the Lomuto partition pivot.
    std::iter_swap(mid, end);
    return end;
}

template <typename RandomIt, typename Compare>
void quick_sort_impl(RandomIt first, RandomIt last, Compare comp) {
    auto distance = std::distance(first, last);
    if (distance <= 1) return;
    if (distance <= 16) {
        // Small ranges: insertion sort avoids recursion overhead and is fast
        // in practice for nearly-sorted tiny partitions.
        insertion_sort(first, last, comp);
        return;
    }

    RandomIt pivot_it = median_of_three_pivot(first, last, comp);
    auto pivot_value = *pivot_it;
    RandomIt store = first;
    for (RandomIt it = first; it != pivot_it; ++it) {
        if (comp(*it, pivot_value)) {
            std::iter_swap(it, store);
            ++store;
        }
    }
    std::iter_swap(store, pivot_it);

    quick_sort_impl(first, store, comp);
    quick_sort_impl(store + 1, last, comp);
}

}  // namespace detail

/**
 * @brief Sorts the range [first, last) using quick sort.
 *
 * Uses median-of-three pivot selection to avoid the classic O(n^2)
 * degradation that a fixed-pivot quicksort exhibits on already-sorted or
 * reverse-sorted input, and falls back to insertion sort for small
 * partitions.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n log n) average and (with median-of-three) typical worst
 * case in practice; true worst case remains O(n^2) for adversarial input,
 * though median-of-three makes naturally-ordered inputs safe.
 * Stable: no (partitioning swaps elements across the pivot).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void quick_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): elige un pivote, particiona el rango alrededor de el
    // y ordena recursivamente las dos particiones resultantes.
    throw std::logic_error("dsa::quick_sort: no implementado");
}

namespace detail {

/**
 * @brief Restores the max-heap (per comp) property for the subtree rooted at
 * index `root` within the logical array [first, first + heap_size), assuming
 * both children subtrees already satisfy the heap property.
 */
template <typename RandomIt, typename Compare>
void sift_down(RandomIt first, std::size_t heap_size, std::size_t root, Compare comp) {
    while (true) {
        std::size_t left = 2 * root + 1;
        std::size_t right = 2 * root + 2;
        std::size_t largest = root;

        if (left < heap_size && comp(*(first + largest), *(first + left))) {
            largest = left;
        }
        if (right < heap_size && comp(*(first + largest), *(first + right))) {
            largest = right;
        }
        if (largest == root) return;

        std::iter_swap(first + root, first + largest);
        root = largest;
    }
}

}  // namespace detail

/**
 * @brief Sorts the range [first, last) using heap sort.
 *
 * Builds an in-place binary max-heap (per comp) over the range and then
 * repeatedly extracts the maximum, placing it at the end. This is a
 * self-contained heapify implementation over iterators; it does not depend
 * on dsa::BinaryHeap.
 *
 * @tparam RandomIt Random access iterator type.
 * @tparam Compare Strict weak ordering predicate type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to one-past-the-end of the range.
 * @param comp Comparator; defaults to std::less (ascending order).
 *
 * Complexity: O(n log n) in the best, average and worst case.
 * Space: O(1) auxiliary (sorts in place).
 * Stable: no (heap extraction reorders equal elements).
 */
template <typename RandomIt,
          typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void heap_sort(RandomIt /*first*/, RandomIt /*last*/, Compare /*comp*/ = Compare()) {
    // TODO(estudiante): construye un max-heap en el rango con detail::sift_down
    // y luego extrae repetidamente el maximo hacia el final del rango.
    throw std::logic_error("dsa::heap_sort: no implementado");
}

}  // namespace dsa

#endif  // DSA_ALGORITHMS_SORTING_HPP
