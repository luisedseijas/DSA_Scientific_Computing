#include "dsa/trees/priority_queue.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

using dsa::PriorityQueue;

// Drains a priority queue with repeated pop() and returns the resulting
// sequence, which must come out ordered by decreasing priority.
template <typename T, typename Compare>
std::vector<T> drain(PriorityQueue<T, Compare>& pq) {
    std::vector<T> out;
    out.reserve(pq.size());
    while (!pq.empty()) {
        out.push_back(pq.pop());
    }
    return out;
}

// ---------------------------------------------------------------------
// Construction / empty state
// ---------------------------------------------------------------------

TEST(PriorityQueueConstruction, DefaultConstructedIsEmpty) {
    PriorityQueue<int> pq;
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);
}

TEST(PriorityQueueConstruction, TopOnEmptyQueueThrows) {
    PriorityQueue<int> pq;
    EXPECT_THROW(pq.top(), std::out_of_range);
}

TEST(PriorityQueueConstruction, PopOnEmptyQueueThrows) {
    PriorityQueue<int> pq;
    EXPECT_THROW(pq.pop(), std::out_of_range);
}

TEST(PriorityQueueConstruction, SingleElement) {
    PriorityQueue<int> pq;
    pq.push(42);
    EXPECT_FALSE(pq.empty());
    EXPECT_EQ(pq.size(), 1u);
    EXPECT_EQ(pq.top(), 42);
    EXPECT_EQ(pq.pop(), 42);
    EXPECT_TRUE(pq.empty());
}

// ---------------------------------------------------------------------
// Max-priority queue (default Compare = std::less<T>)
// ---------------------------------------------------------------------

TEST(PriorityQueueMaxHeap, PushInArbitraryOrderPopsDescending) {
    PriorityQueue<int> pq;
    for (int value : {5, 1, 9, 3, 7, 2, 8, 0, 6, 4}) {
        pq.push(value);
    }
    EXPECT_EQ(drain(pq), (std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

TEST(PriorityQueueMaxHeap, DuplicatesAreHandledCorrectly) {
    PriorityQueue<int> pq;
    for (int value : {3, 3, 1, 1, 2, 2, 3, 1}) {
        pq.push(value);
    }
    EXPECT_EQ(drain(pq), (std::vector<int>{3, 3, 3, 2, 2, 1, 1, 1}));
}

TEST(PriorityQueueMaxHeap, ManyElementsWithDuplicatesProduceSortedOutput) {
    PriorityQueue<int> pq;
    std::vector<int> values;
    for (int i = 0; i < 300; ++i) {
        int value = (i * 53) % 97;
        values.push_back(value);
        pq.push(value);
    }
    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    EXPECT_EQ(drain(pq), expected);
}

// ---------------------------------------------------------------------
// Min-priority queue (Compare = std::greater<T>)
// ---------------------------------------------------------------------

TEST(PriorityQueueMinHeap, PushInArbitraryOrderPopsAscending) {
    PriorityQueue<int, std::greater<int>> pq;
    for (int value : {5, 1, 9, 3, 7, 2, 8, 0, 6, 4}) {
        pq.push(value);
    }
    EXPECT_EQ(drain(pq), (std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

// ---------------------------------------------------------------------
// Construction from an iterator range (delegates to BinaryHeap::build_heap)
// ---------------------------------------------------------------------

TEST(PriorityQueueBuildFromRange, FromUnsortedVectorMaxHeap) {
    std::vector<int> values{4, 10, 3, 5, 1, 8, 2, 9, 7, 6};
    PriorityQueue<int> pq(values.begin(), values.end());
    EXPECT_EQ(pq.size(), values.size());
    EXPECT_EQ(drain(pq), (std::vector<int>{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}));
}

TEST(PriorityQueueBuildFromRange, FromUnsortedVectorMinHeap) {
    std::vector<int> values{4, 10, 3, 5, 1, 8, 2, 9, 7, 6};
    PriorityQueue<int, std::greater<int>> pq(values.begin(), values.end());
    EXPECT_EQ(drain(pq), (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
}

TEST(PriorityQueueBuildFromRange, FromEmptyRangeIsEmpty) {
    std::vector<int> values;
    PriorityQueue<int> pq(values.begin(), values.end());
    EXPECT_TRUE(pq.empty());
}

// ---------------------------------------------------------------------
// clear()
// ---------------------------------------------------------------------

TEST(PriorityQueueClear, ResetsSizeAndIsUsableAfterward) {
    PriorityQueue<int> pq;
    pq.push(1);
    pq.push(2);
    pq.push(3);
    pq.clear();
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0u);
    pq.push(10);
    pq.push(5);
    EXPECT_EQ(drain(pq), (std::vector<int>{10, 5}));
}

// ---------------------------------------------------------------------
// Copy / move semantics (inherited from BinaryHeap via defaulted members)
// ---------------------------------------------------------------------

TEST(PriorityQueueCopyMove, CopyIsIndependent) {
    PriorityQueue<int> original;
    original.push(1);
    original.push(5);
    original.push(3);
    PriorityQueue<int> copy(original);
    copy.push(100);
    EXPECT_NE(original.size(), copy.size());
    EXPECT_EQ(copy.top(), 100);
    EXPECT_EQ(original.top(), 5);
}

TEST(PriorityQueueCopyMove, MoveConstructorTransfersOwnership) {
    PriorityQueue<int> original;
    original.push(1);
    original.push(5);
    PriorityQueue<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 2u);
    EXPECT_EQ(moved.top(), 5);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(PriorityQueueCopyMove, SwapExchangesContents) {
    PriorityQueue<int> a;
    a.push(1);
    a.push(2);
    PriorityQueue<int> b;
    b.push(10);
    b.push(20);
    b.push(30);
    a.swap(b);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a.top(), 30);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.top(), 2);
}

}  // namespace
