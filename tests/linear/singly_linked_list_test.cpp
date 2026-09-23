#include "dsa/linear/singly_linked_list.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using dsa::SinglyLinkedList;

TEST(SinglyLinkedList, StartsEmpty) {
    SinglyLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(SinglyLinkedList, PushBackIncreasesSizeAndOrder) {
    SinglyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    ASSERT_EQ(list.size(), 3u);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
}

TEST(SinglyLinkedList, PushFrontPrepends) {
    SinglyLinkedList<int> list;
    list.push_back(2);
    list.push_front(1);
    list.push_front(0);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{0, 1, 2}));
}

TEST(SinglyLinkedList, PopFrontRemovesHead) {
    SinglyLinkedList<int> list{1, 2, 3};
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.size(), 2u);
}

TEST(SinglyLinkedList, PopBackRemovesTailAndFixesTailPointer) {
    SinglyLinkedList<int> list{1, 2, 3};
    list.pop_back();
    EXPECT_EQ(list.back(), 2);
    list.push_back(9);
    EXPECT_EQ(list.back(), 9);
    EXPECT_EQ(list.size(), 3u);
}

TEST(SinglyLinkedList, PopOnEmptyThrows) {
    SinglyLinkedList<int> list;
    EXPECT_THROW(list.pop_front(), std::out_of_range);
    EXPECT_THROW(list.pop_back(), std::out_of_range);
}

TEST(SinglyLinkedList, SingleNodeLifecycle) {
    SinglyLinkedList<int> list;
    list.push_back(42);
    EXPECT_EQ(list.front(), 42);
    EXPECT_EQ(list.back(), 42);
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

TEST(SinglyLinkedList, InsertAtMiddle) {
    SinglyLinkedList<int> list{1, 2, 4};
    list.insert_at(2, 3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4}));
}

TEST(SinglyLinkedList, InsertAtBoundaries) {
    SinglyLinkedList<int> list{2};
    list.insert_at(0, 1);
    list.insert_at(2, 3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST(SinglyLinkedList, InsertAtOutOfRangeThrows) {
    SinglyLinkedList<int> list{1, 2};
    EXPECT_THROW(list.insert_at(5, 99), std::out_of_range);
}

TEST(SinglyLinkedList, EraseAtMiddle) {
    SinglyLinkedList<int> list{1, 2, 3, 4};
    list.erase_at(1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 3, 4}));
    EXPECT_EQ(list.size(), 3u);
}

TEST(SinglyLinkedList, EraseAtLastFixesTail) {
    SinglyLinkedList<int> list{1, 2, 3};
    list.erase_at(2);
    EXPECT_EQ(list.back(), 2);
    list.push_back(9);
    EXPECT_EQ(list.back(), 9);
}

TEST(SinglyLinkedList, EraseAtOutOfRangeThrows) {
    SinglyLinkedList<int> list{1};
    EXPECT_THROW(list.erase_at(1), std::out_of_range);
}

TEST(SinglyLinkedList, Find) {
    SinglyLinkedList<int> list{1, 2, 3};
    auto it = list.find(2);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(list.find(99), list.end());
}

TEST(SinglyLinkedList, IterationVisitsAllElementsInOrder) {
    SinglyLinkedList<int> list{10, 20, 30};
    int expected = 10;
    for (int value : list) {
        EXPECT_EQ(value, expected);
        expected += 10;
    }
}

TEST(SinglyLinkedList, CopyConstructorIsDeep) {
    SinglyLinkedList<int> original{1, 2, 3};
    SinglyLinkedList<int> copy(original);
    copy.push_back(4);
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
}

TEST(SinglyLinkedList, CopyAssignment) {
    SinglyLinkedList<int> original{1, 2, 3};
    SinglyLinkedList<int> other{99};
    other = original;
    EXPECT_EQ(other.size(), 3u);
    EXPECT_EQ(other.front(), 1);
    original.push_back(4);
    EXPECT_EQ(other.size(), 3u);
}

TEST(SinglyLinkedList, MoveConstructorLeavesSourceEmpty) {
    SinglyLinkedList<int> original{1, 2, 3};
    SinglyLinkedList<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(SinglyLinkedList, MoveAssignment) {
    SinglyLinkedList<int> original{1, 2, 3};
    SinglyLinkedList<int> other;
    other = std::move(original);
    EXPECT_EQ(other.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(SinglyLinkedList, ClearEmptiesListAndAllowsReuse) {
    SinglyLinkedList<std::string> list{"a", "b", "c"};
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
    list.push_back("z");
    EXPECT_EQ(list.front(), "z");
}

TEST(SinglyLinkedList, FrontBackOnEmptyThrows) {
    SinglyLinkedList<int> list;
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
}
