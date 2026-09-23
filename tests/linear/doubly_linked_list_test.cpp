#include "dsa/linear/doubly_linked_list.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using dsa::DoublyLinkedList;

TEST(DoublyLinkedList, StartsEmpty) {
    DoublyLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(DoublyLinkedList, PushBackAndPushFront) {
    DoublyLinkedList<int> list;
    list.push_back(2);
    list.push_back(3);
    list.push_front(1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
}

TEST(DoublyLinkedList, PopFrontAndPopBackAreConstantTime) {
    DoublyLinkedList<int> list{1, 2, 3, 4};
    list.pop_front();
    list.pop_back();
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{2, 3}));
}

TEST(DoublyLinkedList, PopBackRepeatedlyDrainsList) {
    DoublyLinkedList<int> list{1, 2, 3};
    list.pop_back();
    list.pop_back();
    list.pop_back();
    EXPECT_TRUE(list.empty());
    EXPECT_THROW(list.pop_back(), std::out_of_range);
}

TEST(DoublyLinkedList, PopOnEmptyThrows) {
    DoublyLinkedList<int> list;
    EXPECT_THROW(list.pop_front(), std::out_of_range);
    EXPECT_THROW(list.pop_back(), std::out_of_range);
}

TEST(DoublyLinkedList, SingleNodeLifecycle) {
    DoublyLinkedList<int> list;
    list.push_back(7);
    EXPECT_EQ(list.front(), 7);
    EXPECT_EQ(list.back(), 7);
    list.pop_back();
    EXPECT_TRUE(list.empty());
}

TEST(DoublyLinkedList, InsertAtMiddle) {
    DoublyLinkedList<int> list{1, 2, 4};
    list.insert_at(2, 3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4}));
}

TEST(DoublyLinkedList, InsertAtBoundaries) {
    DoublyLinkedList<int> list{2};
    list.insert_at(0, 1);
    list.insert_at(2, 3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST(DoublyLinkedList, EraseAtMiddleFixesBothLinks) {
    DoublyLinkedList<int> list{1, 2, 3, 4};
    list.erase_at(1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 3, 4}));

    // Traversing backwards should also reflect the removal.
    std::vector<int> reversed;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        reversed.push_back(*it);
    }
    EXPECT_EQ(reversed, (std::vector<int>{4, 3, 1}));
}

TEST(DoublyLinkedList, EraseAtEndpointsUpdatesHeadAndTail) {
    DoublyLinkedList<int> list{1, 2, 3};
    list.erase_at(0);
    EXPECT_EQ(list.front(), 2);
    list.erase_at(1);
    EXPECT_EQ(list.back(), 2);
    EXPECT_EQ(list.size(), 1u);
}

TEST(DoublyLinkedList, EraseAtOutOfRangeThrows) {
    DoublyLinkedList<int> list{1};
    EXPECT_THROW(list.erase_at(5), std::out_of_range);
}

TEST(DoublyLinkedList, Find) {
    DoublyLinkedList<int> list{1, 2, 3};
    auto it = list.find(3);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(list.find(42), list.end());
}

TEST(DoublyLinkedList, ReverseIteration) {
    DoublyLinkedList<int> list{1, 2, 3};
    std::vector<int> reversed;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        reversed.push_back(*it);
    }
    EXPECT_EQ(reversed, (std::vector<int>{3, 2, 1}));
}

TEST(DoublyLinkedList, CopyConstructorIsDeep) {
    DoublyLinkedList<int> original{1, 2, 3};
    DoublyLinkedList<int> copy(original);
    copy.push_back(4);
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
}

TEST(DoublyLinkedList, CopyAssignment) {
    DoublyLinkedList<int> original{1, 2, 3};
    DoublyLinkedList<int> other{99};
    other = original;
    EXPECT_EQ(other.size(), 3u);
    original.push_back(4);
    EXPECT_EQ(other.size(), 3u);
}

TEST(DoublyLinkedList, MoveConstructorLeavesSourceEmpty) {
    DoublyLinkedList<int> original{1, 2, 3};
    DoublyLinkedList<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(DoublyLinkedList, MoveAssignment) {
    DoublyLinkedList<int> original{1, 2, 3};
    DoublyLinkedList<int> other;
    other = std::move(original);
    EXPECT_EQ(other.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(DoublyLinkedList, ClearEmptiesListAndAllowsReuse) {
    DoublyLinkedList<std::string> list{"a", "b", "c"};
    list.clear();
    EXPECT_TRUE(list.empty());
    list.push_back("z");
    EXPECT_EQ(list.front(), "z");
}

TEST(DoublyLinkedList, FrontBackOnEmptyThrows) {
    DoublyLinkedList<int> list;
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
}
