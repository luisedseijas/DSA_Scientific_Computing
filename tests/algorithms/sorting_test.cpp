#include "dsa/algorithms/sorting.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

namespace {

template <typename Fn>
void ExpectSorted(Fn sort_fn, std::vector<int> input) {
    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    sort_fn(input.begin(), input.end());
    EXPECT_EQ(input, expected);
}

template <typename Fn>
void ExpectSortedDescending(Fn sort_fn, std::vector<int> input) {
    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    sort_fn(input.begin(), input.end(), std::greater<int>());
    EXPECT_EQ(input, expected);
}

std::vector<int> RandomVector(std::size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

}  // namespace

// Macro to generate the standard suite of cases for a sort function.
#define SORTING_TEST_SUITE(FnName)                                                          \
    namespace {                                                                             \
    void FnName##_call(std::vector<int>::iterator first, std::vector<int>::iterator last) { \
        dsa::FnName(first, last);                                                           \
    }                                                                                       \
    }                                                                                       \
    TEST(FnName, EmptyRange) { ExpectSorted(FnName##_call, {}); }                           \
    TEST(FnName, SingleElement) { ExpectSorted(FnName##_call, {42}); }                      \
    TEST(FnName, AlreadySorted) { ExpectSorted(FnName##_call, {1, 2, 3, 4, 5}); }           \
    TEST(FnName, ReverseSorted) { ExpectSorted(FnName##_call, {5, 4, 3, 2, 1}); }           \
    TEST(FnName, WithDuplicates) { ExpectSorted(FnName##_call, {3, 1, 2, 3, 1, 2, 3}); }    \
    TEST(FnName, LargeRandom) {                                                             \
        auto v = RandomVector(2000, 12345);                                                 \
        ExpectSorted(FnName##_call, v);                                                     \
    }                                                                                       \
    TEST(FnName, CustomComparatorDescending) {                                              \
        ExpectSortedDescending(                                                             \
            static_cast<void (*)(std::vector<int>::iterator, std::vector<int>::iterator,    \
                                 std::greater<int>)>(dsa::FnName),                          \
            {3, 1, 4, 1, 5, 9, 2, 6});                                                      \
    }

SORTING_TEST_SUITE(bubble_sort)
SORTING_TEST_SUITE(insertion_sort)
SORTING_TEST_SUITE(selection_sort)
SORTING_TEST_SUITE(merge_sort)
SORTING_TEST_SUITE(quick_sort)
SORTING_TEST_SUITE(heap_sort)

TEST(SortingAlgorithms, LargeRandomIsSortedAllAlgorithms) {
    auto base = RandomVector(5000, 999);

    auto v1 = base;
    dsa::bubble_sort(v1.begin(), v1.end());
    EXPECT_TRUE(std::is_sorted(v1.begin(), v1.end()));

    auto v2 = base;
    dsa::insertion_sort(v2.begin(), v2.end());
    EXPECT_TRUE(std::is_sorted(v2.begin(), v2.end()));

    auto v3 = base;
    dsa::selection_sort(v3.begin(), v3.end());
    EXPECT_TRUE(std::is_sorted(v3.begin(), v3.end()));

    auto v4 = base;
    dsa::merge_sort(v4.begin(), v4.end());
    EXPECT_TRUE(std::is_sorted(v4.begin(), v4.end()));

    auto v5 = base;
    dsa::quick_sort(v5.begin(), v5.end());
    EXPECT_TRUE(std::is_sorted(v5.begin(), v5.end()));

    auto v6 = base;
    dsa::heap_sort(v6.begin(), v6.end());
    EXPECT_TRUE(std::is_sorted(v6.begin(), v6.end()));
}

TEST(QuickSort, DoesNotDegradeOnAlreadySortedLargeInput) {
    std::vector<int> v(3000);
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = static_cast<int>(i);
    dsa::quick_sort(v.begin(), v.end());
    EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
}
