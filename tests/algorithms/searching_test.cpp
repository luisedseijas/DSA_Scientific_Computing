#include "dsa/algorithms/searching.hpp"

#include <gtest/gtest.h>

#include <list>
#include <vector>

// ---------------------------------------------------------------------------
// linear_search
// ---------------------------------------------------------------------------

TEST(LinearSearch, EmptyRangeReturnsLast) {
    std::vector<int> v;
    EXPECT_EQ(dsa::linear_search(v.begin(), v.end(), 5), v.end());
}

TEST(LinearSearch, FindsElementAtBeginning) {
    std::vector<int> v{1, 2, 3, 4, 5};
    auto it = dsa::linear_search(v.begin(), v.end(), 1);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 1);
}

TEST(LinearSearch, FindsElementInMiddle) {
    std::vector<int> v{1, 2, 3, 4, 5};
    auto it = dsa::linear_search(v.begin(), v.end(), 3);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 3);
}

TEST(LinearSearch, FindsElementAtEnd) {
    std::vector<int> v{1, 2, 3, 4, 5};
    auto it = dsa::linear_search(v.begin(), v.end(), 5);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 5);
}

TEST(LinearSearch, AbsentElementReturnsLast) {
    std::vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(dsa::linear_search(v.begin(), v.end(), 42), v.end());
}

TEST(LinearSearch, WorksOnNonRandomAccessIterators) {
    std::list<int> l{10, 20, 30};
    auto it = dsa::linear_search(l.begin(), l.end(), 20);
    ASSERT_NE(it, l.end());
    EXPECT_EQ(*it, 20);
}

// ---------------------------------------------------------------------------
// binary_search (iterative)
// ---------------------------------------------------------------------------

TEST(BinarySearch, EmptyRangeReturnsLast) {
    std::vector<int> v;
    EXPECT_EQ(dsa::binary_search(v.begin(), v.end(), 5), v.end());
}

TEST(BinarySearch, SingleElementFound) {
    std::vector<int> v{7};
    auto it = dsa::binary_search(v.begin(), v.end(), 7);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 7);
}

TEST(BinarySearch, SingleElementNotFound) {
    std::vector<int> v{7};
    EXPECT_EQ(dsa::binary_search(v.begin(), v.end(), 1), v.end());
}

TEST(BinarySearch, FindsElementAtBeginning) {
    std::vector<int> v{1, 3, 5, 7, 9, 11};
    auto it = dsa::binary_search(v.begin(), v.end(), 1);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 1);
}

TEST(BinarySearch, FindsElementInMiddle) {
    std::vector<int> v{1, 3, 5, 7, 9, 11};
    auto it = dsa::binary_search(v.begin(), v.end(), 7);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 7);
}

TEST(BinarySearch, FindsElementAtEnd) {
    std::vector<int> v{1, 3, 5, 7, 9, 11};
    auto it = dsa::binary_search(v.begin(), v.end(), 11);
    ASSERT_NE(it, v.end());
    EXPECT_EQ(*it, 11);
}

TEST(BinarySearch, AbsentElementReturnsLast) {
    std::vector<int> v{1, 3, 5, 7, 9, 11};
    EXPECT_EQ(dsa::binary_search(v.begin(), v.end(), 4), v.end());
}

TEST(BinarySearch, LargeSortedRangeAllElementsFound) {
    std::vector<int> v;
    for (int i = 0; i < 10000; i += 2) v.push_back(i);
    for (int i = 0; i < 10000; i += 2) {
        auto it = dsa::binary_search(v.begin(), v.end(), i);
        ASSERT_NE(it, v.end());
        EXPECT_EQ(*it, i);
    }
    EXPECT_EQ(dsa::binary_search(v.begin(), v.end(), 1), v.end());
}

// ---------------------------------------------------------------------------
// binary_search_recursive
// ---------------------------------------------------------------------------

TEST(BinarySearchRecursive, EmptyRangeReturnsLast) {
    std::vector<int> v;
    EXPECT_EQ(dsa::binary_search_recursive(v.begin(), v.end(), 5), v.end());
}

TEST(BinarySearchRecursive, FindsElementAtBeginningMiddleEnd) {
    std::vector<int> v{2, 4, 6, 8, 10, 12, 14};

    auto begin_it = dsa::binary_search_recursive(v.begin(), v.end(), 2);
    ASSERT_NE(begin_it, v.end());
    EXPECT_EQ(*begin_it, 2);

    auto mid_it = dsa::binary_search_recursive(v.begin(), v.end(), 8);
    ASSERT_NE(mid_it, v.end());
    EXPECT_EQ(*mid_it, 8);

    auto end_it = dsa::binary_search_recursive(v.begin(), v.end(), 14);
    ASSERT_NE(end_it, v.end());
    EXPECT_EQ(*end_it, 14);
}

TEST(BinarySearchRecursive, AbsentElementReturnsLast) {
    std::vector<int> v{2, 4, 6, 8, 10, 12, 14};
    EXPECT_EQ(dsa::binary_search_recursive(v.begin(), v.end(), 5), v.end());
}

TEST(BinarySearchRecursive, MatchesIterativeResultOnLargeRange) {
    std::vector<int> v;
    for (int i = 0; i < 5000; ++i) v.push_back(i * 3);

    for (int i = 0; i < 15000; i += 7) {
        auto iterative = dsa::binary_search(v.begin(), v.end(), i);
        auto recursive = dsa::binary_search_recursive(v.begin(), v.end(), i);
        EXPECT_EQ(iterative == v.end(), recursive == v.end());
        if (iterative != v.end() && recursive != v.end()) {
            EXPECT_EQ(*iterative, *recursive);
        }
    }
}
