#include "dsa/linear/dynamic_array.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <utility>

namespace {

using dsa::DynamicArray;

// ---------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------

TEST(DynamicArrayConstruction, DefaultConstructedIsEmpty) {
    DynamicArray<int> arr;
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_EQ(arr.capacity(), 0u);
}

TEST(DynamicArrayConstruction, InitialCapacityReservesStorageButStaysEmpty) {
    DynamicArray<int> arr(16);
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_EQ(arr.capacity(), 16u);
}

TEST(DynamicArrayConstruction, ZeroInitialCapacityIsEquivalentToDefault) {
    DynamicArray<int> arr(0);
    EXPECT_EQ(arr.capacity(), 0u);
}

// ---------------------------------------------------------------------
// push_back / pop_back
// ---------------------------------------------------------------------

TEST(DynamicArrayPushPop, PushBackIncreasesSizeAndPreservesOrder) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayPushPop, PushBackMoveOverloadWorksForMoveOnlyLikeTypes) {
    DynamicArray<std::string> arr;
    std::string s = "hello world, this is a longer string to avoid SSO";
    arr.push_back(std::move(s));
    EXPECT_EQ(arr[0], "hello world, this is a longer string to avoid SSO");
}

TEST(DynamicArrayPushPop, PopBackRemovesLastElement) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.pop_back();

    ASSERT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[0], 1);
}

TEST(DynamicArrayPushPop, PopBackOnEmptyThrowsOutOfRange) {
    DynamicArray<int> arr;
    EXPECT_THROW(arr.pop_back(), std::out_of_range);
}

TEST(DynamicArrayPushPop, PushBackThenPopBackToEmptyKeepsCapacity) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    const std::size_t cap_before = arr.capacity();
    arr.pop_back();
    arr.pop_back();
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.capacity(), cap_before);
}

// ---------------------------------------------------------------------
// Capacity growth
// ---------------------------------------------------------------------

TEST(DynamicArrayGrowth, CapacityDoublesWhenFull) {
    DynamicArray<int> arr;
    std::size_t last_capacity = arr.capacity();
    int growth_events = 0;

    for (int i = 0; i < 100; ++i) {
        arr.push_back(i);
        if (arr.capacity() != last_capacity) {
            if (last_capacity != 0) {
                EXPECT_EQ(arr.capacity(), last_capacity * 2);
            }
            last_capacity = arr.capacity();
            ++growth_events;
        }
    }

    // Growth should be logarithmic in the number of insertions, not linear.
    EXPECT_LT(growth_events, 20);
    EXPECT_GE(arr.capacity(), 100u);
}

TEST(DynamicArrayGrowth, ElementsSurviveReallocation) {
    DynamicArray<int> arr;
    for (int i = 0; i < 1000; ++i) {
        arr.push_back(i);
    }
    ASSERT_EQ(arr.size(), 1000u);
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(arr[static_cast<std::size_t>(i)], i);
    }
}

// ---------------------------------------------------------------------
// insert / erase
// ---------------------------------------------------------------------

TEST(DynamicArrayInsertErase, InsertAtBeginning) {
    DynamicArray<int> arr;
    arr.push_back(2);
    arr.push_back(3);
    arr.insert(0, 1);

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayInsertErase, InsertInMiddle) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(3);
    arr.push_back(4);
    arr.insert(1, 2);

    ASSERT_EQ(arr.size(), 4u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
}

TEST(DynamicArrayInsertErase, InsertAtEndBehavesLikePushBack) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.insert(arr.size(), 3);

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr.back(), 3);
}

TEST(DynamicArrayInsertErase, InsertIntoEmptyArray) {
    DynamicArray<int> arr;
    arr.insert(0, 42);
    ASSERT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[0], 42);
}

TEST(DynamicArrayInsertErase, InsertOutOfRangeThrows) {
    DynamicArray<int> arr;
    arr.push_back(1);
    EXPECT_THROW(arr.insert(5, 99), std::out_of_range);
}

TEST(DynamicArrayInsertErase, EraseFromMiddle) {
    DynamicArray<int> arr;
    for (int i = 0; i < 5; ++i) arr.push_back(i);  // 0 1 2 3 4
    arr.erase(2);                                  // removes '2'

    ASSERT_EQ(arr.size(), 4u);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
}

TEST(DynamicArrayInsertErase, EraseFirstAndLast) {
    DynamicArray<int> arr;
    for (int i = 0; i < 3; ++i) arr.push_back(i);  // 0 1 2
    arr.erase(0);                                  // 1 2
    arr.erase(arr.size() - 1);                     // 1

    ASSERT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[0], 1);
}

TEST(DynamicArrayInsertErase, EraseOutOfRangeThrows) {
    DynamicArray<int> arr;
    arr.push_back(1);
    EXPECT_THROW(arr.erase(1), std::out_of_range);
    EXPECT_THROW(arr.erase(100), std::out_of_range);
}

// ---------------------------------------------------------------------
// Element access: operator[], at(), front(), back()
// ---------------------------------------------------------------------

TEST(DynamicArrayAccess, AtThrowsOutOfRange) {
    DynamicArray<int> arr;
    arr.push_back(1);
    EXPECT_NO_THROW(arr.at(0));
    EXPECT_THROW(arr.at(1), std::out_of_range);
}

TEST(DynamicArrayAccess, ConstAtThrowsOutOfRange) {
    DynamicArray<int> arr;
    arr.push_back(1);
    const DynamicArray<int>& const_arr = arr;
    EXPECT_NO_THROW(const_arr.at(0));
    EXPECT_THROW(const_arr.at(1), std::out_of_range);
}

TEST(DynamicArrayAccess, FrontAndBack) {
    DynamicArray<int> arr;
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);

    EXPECT_EQ(arr.front(), 10);
    EXPECT_EQ(arr.back(), 30);

    arr.front() = 100;
    arr.back() = 300;
    EXPECT_EQ(arr[0], 100);
    EXPECT_EQ(arr[2], 300);
}

TEST(DynamicArrayAccess, OperatorBracketAllowsMutation) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr[0] = 99;
    EXPECT_EQ(arr[0], 99);
}

// ---------------------------------------------------------------------
// clear / empty
// ---------------------------------------------------------------------

TEST(DynamicArrayClear, ClearEmptiesButKeepsCapacity) {
    DynamicArray<int> arr;
    for (int i = 0; i < 10; ++i) arr.push_back(i);
    const std::size_t cap_before = arr.capacity();

    arr.clear();
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0u);
    EXPECT_EQ(arr.capacity(), cap_before);
}

TEST(DynamicArrayClear, ClearOnEmptyArrayIsNoop) {
    DynamicArray<int> arr;
    EXPECT_NO_THROW(arr.clear());
    EXPECT_TRUE(arr.empty());
}

// ---------------------------------------------------------------------
// Copy semantics
// ---------------------------------------------------------------------

TEST(DynamicArrayCopy, CopyConstructorDeepCopies) {
    DynamicArray<int> original;
    for (int i = 0; i < 5; ++i) original.push_back(i);

    DynamicArray<int> copy(original);
    copy[0] = 999;

    EXPECT_EQ(original[0], 0);  // original untouched
    EXPECT_EQ(copy[0], 999);
    EXPECT_EQ(original.size(), copy.size());
}

TEST(DynamicArrayCopy, CopyAssignmentDeepCopies) {
    DynamicArray<int> original;
    for (int i = 0; i < 5; ++i) original.push_back(i);

    DynamicArray<int> other;
    other.push_back(-1);
    other = original;
    other[1] = 999;

    EXPECT_EQ(original[1], 1);
    EXPECT_EQ(other[1], 999);
}

TEST(DynamicArrayCopy, SelfCopyAssignmentIsSafe) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);

    arr = arr;

    ASSERT_EQ(arr.size(), 2u);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
}

// ---------------------------------------------------------------------
// Move semantics
// ---------------------------------------------------------------------

TEST(DynamicArrayMove, MoveConstructorStealsBuffer) {
    DynamicArray<int> original;
    for (int i = 0; i < 5; ++i) original.push_back(i);

    DynamicArray<int> moved(std::move(original));

    EXPECT_EQ(moved.size(), 5u);
    EXPECT_EQ(moved[0], 0);
    EXPECT_TRUE(original.empty());       // NOLINT(bugprone-use-after-move)
    EXPECT_EQ(original.capacity(), 0u);  // NOLINT(bugprone-use-after-move)
}

TEST(DynamicArrayMove, MoveAssignmentStealsBuffer) {
    DynamicArray<int> original;
    for (int i = 0; i < 5; ++i) original.push_back(i);

    DynamicArray<int> other;
    other.push_back(-1);
    other = std::move(original);

    EXPECT_EQ(other.size(), 5u);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

// ---------------------------------------------------------------------
// Iteration: range-for and <algorithm> compatibility
// ---------------------------------------------------------------------

TEST(DynamicArrayIteration, RangeForVisitsAllElementsInOrder) {
    DynamicArray<int> arr;
    for (int i = 0; i < 5; ++i) arr.push_back(i);

    int expected = 0;
    for (int value : arr) {
        EXPECT_EQ(value, expected);
        ++expected;
    }
    EXPECT_EQ(expected, 5);
}

TEST(DynamicArrayIteration, RangeForOnConstArray) {
    DynamicArray<int> arr;
    for (int i = 0; i < 3; ++i) arr.push_back(i * 2);

    const DynamicArray<int>& const_arr = arr;
    int sum = 0;
    for (int value : const_arr) {
        sum += value;
    }
    EXPECT_EQ(sum, 0 + 2 + 4);
}

TEST(DynamicArrayIteration, StdSortWorksViaBeginEnd) {
    DynamicArray<int> arr;
    arr.push_back(5);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(1);
    arr.push_back(2);

    std::sort(arr.begin(), arr.end());

    for (std::size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], static_cast<int>(i) + 1);
    }
}

TEST(DynamicArrayIteration, StdAccumulateWorks) {
    DynamicArray<int> arr;
    for (int i = 1; i <= 5; ++i) arr.push_back(i);

    int sum = std::accumulate(arr.begin(), arr.end(), 0);
    EXPECT_EQ(sum, 15);
}

TEST(DynamicArrayIteration, StdFindWorks) {
    DynamicArray<int> arr;
    for (int i = 0; i < 10; ++i) arr.push_back(i);

    auto it = std::find(arr.begin(), arr.end(), 7);
    ASSERT_NE(it, arr.end());
    EXPECT_EQ(*it, 7);
}

TEST(DynamicArrayIteration, EmptyArrayBeginEqualsEnd) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.begin(), arr.end());
}

// ---------------------------------------------------------------------
// Edge cases: empty array, single element
// ---------------------------------------------------------------------

TEST(DynamicArrayEdgeCases, SingleElementFrontEqualsBack) {
    DynamicArray<int> arr;
    arr.push_back(42);
    EXPECT_EQ(arr.front(), arr.back());
    EXPECT_EQ(arr.size(), 1u);
}

TEST(DynamicArrayEdgeCases, EraseOnlyElementLeavesArrayEmpty) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.erase(0);
    EXPECT_TRUE(arr.empty());
}

TEST(DynamicArrayEdgeCases, InsertIntoSingleElementArray) {
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.insert(0, 0);
    arr.insert(2, 2);

    ASSERT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 2);
}

TEST(DynamicArrayEdgeCases, ClearOnEmptyThenPushBackWorks) {
    DynamicArray<int> arr;
    arr.clear();
    arr.push_back(1);
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[0], 1);
}

}  // namespace
