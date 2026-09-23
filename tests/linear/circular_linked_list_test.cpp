#include "dsa/linear/circular_linked_list.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using dsa::CircularLinkedList;

TEST(CircularLinkedList, StartsEmpty) {
    CircularLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(CircularLinkedList, PushBackAppendsInOrder) {
    CircularLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 3);
}

TEST(CircularLinkedList, PushFrontPrepends) {
    CircularLinkedList<int> list;
    list.push_back(2);
    list.push_back(3);
    list.push_front(1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST(CircularLinkedList, PopFrontRemovesHeadAndKeepsCircularity) {
    CircularLinkedList<int> list{1, 2, 3};
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.back(), 3);
    EXPECT_EQ(list.size(), 2u);
}

TEST(CircularLinkedList, PopOnEmptyThrows) {
    CircularLinkedList<int> list;
    EXPECT_THROW(list.pop_front(), std::out_of_range);
}

TEST(CircularLinkedList, SingleNodeLifecycle) {
    CircularLinkedList<int> list;
    list.push_back(5);
    EXPECT_EQ(list.front(), 5);
    EXPECT_EQ(list.back(), 5);
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

TEST(CircularLinkedList, IterationTerminatesAfterSizeElements) {
    // El punto central de esta prueba: la cadena subyacente no tiene un
    // centinela nullptr, así que la iteración debe detenerse por conteo,
    // no por comparación de punteros.
    CircularLinkedList<int> list{1, 2, 3};
    std::size_t visited = 0;
    for (int value : list) {
        (void)value;
        ++visited;
        ASSERT_LE(visited, 3u) << "la iteración no terminó";
    }
    EXPECT_EQ(visited, 3u);
}

TEST(CircularLinkedList, IterationOnEmptyListVisitsNothing) {
    CircularLinkedList<int> list;
    std::size_t visited = 0;
    for (int value : list) {
        (void)value;
        ++visited;
    }
    EXPECT_EQ(visited, 0u);
}

TEST(CircularLinkedList, RotatePositiveMovesHeadForward) {
    CircularLinkedList<int> list{1, 2, 3, 4};
    list.rotate(1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{2, 3, 4, 1}));
}

TEST(CircularLinkedList, RotateNegativeMovesHeadBackward) {
    CircularLinkedList<int> list{1, 2, 3, 4};
    list.rotate(-1);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{4, 1, 2, 3}));
}

TEST(CircularLinkedList, RotateByFullSizeIsIdentity) {
    CircularLinkedList<int> list{1, 2, 3};
    list.rotate(3);
    std::vector<int> values(list.begin(), list.end());
    EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST(CircularLinkedList, RotateOnEmptyListIsNoop) {
    CircularLinkedList<int> list;
    EXPECT_NO_THROW(list.rotate(5));
    EXPECT_TRUE(list.empty());
}

TEST(CircularLinkedList, Find) {
    CircularLinkedList<int> list{1, 2, 3};
    auto it = list.find(2);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(list.find(99), list.end());
}

TEST(CircularLinkedList, CopyConstructorIsDeep) {
    CircularLinkedList<int> original{1, 2, 3};
    CircularLinkedList<int> copy(original);
    copy.push_back(4);
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
}

TEST(CircularLinkedList, CopyAssignment) {
    CircularLinkedList<int> original{1, 2, 3};
    CircularLinkedList<int> other{99};
    other = original;
    EXPECT_EQ(other.size(), 3u);
    original.push_back(4);
    EXPECT_EQ(other.size(), 3u);
}

TEST(CircularLinkedList, MoveConstructorLeavesSourceEmpty) {
    CircularLinkedList<int> original{1, 2, 3};
    CircularLinkedList<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(CircularLinkedList, MoveAssignment) {
    CircularLinkedList<int> original{1, 2, 3};
    CircularLinkedList<int> other;
    other = std::move(original);
    EXPECT_EQ(other.size(), 3u);
    EXPECT_TRUE(original.empty());
}

TEST(CircularLinkedList, ClearEmptiesListAndAllowsReuse) {
    CircularLinkedList<std::string> list{"a", "b", "c"};
    list.clear();
    EXPECT_TRUE(list.empty());
    list.push_back("z");
    EXPECT_EQ(list.front(), "z");
}

TEST(CircularLinkedList, FrontBackOnEmptyThrows) {
    CircularLinkedList<int> list;
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
}
