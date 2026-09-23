#include "dsa/linear/deque.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using dsa::Deque;

TEST(DequeTest, StartsEmpty) {
    Deque<int> d;
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0u);
}

TEST(DequeTest, PushBackAndFront) {
    Deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_front(0);
    EXPECT_EQ(d.size(), 3u);
    EXPECT_EQ(d.front(), 0);
    EXPECT_EQ(d.back(), 2);
}

TEST(DequeTest, PopFrontAndBack) {
    Deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);
    EXPECT_EQ(d.front(), 1);
    d.pop_front();
    EXPECT_EQ(d.front(), 2);
    EXPECT_EQ(d.back(), 3);
    d.pop_back();
    EXPECT_EQ(d.back(), 2);
    EXPECT_EQ(d.size(), 1u);
    d.pop_front();
    EXPECT_TRUE(d.empty());
}

TEST(DequeTest, FrontThrowsOnEmpty) {
    Deque<int> d;
    EXPECT_THROW(d.front(), std::out_of_range);
}

TEST(DequeTest, BackThrowsOnEmpty) {
    Deque<int> d;
    EXPECT_THROW(d.back(), std::out_of_range);
}

TEST(DequeTest, PopFrontThrowsOnEmpty) {
    Deque<int> d;
    EXPECT_THROW(d.pop_front(), std::out_of_range);
}

TEST(DequeTest, PopBackThrowsOnEmpty) {
    Deque<int> d;
    EXPECT_THROW(d.pop_back(), std::out_of_range);
}

TEST(DequeTest, ClearEmptiesButKeepsCapacity) {
    Deque<int> d;
    for (int i = 0; i < 10; ++i) {
        d.push_back(i);
    }
    std::size_t cap_before = d.capacity();
    d.clear();
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.capacity(), cap_before);
    EXPECT_THROW(d.front(), std::out_of_range);
}

// Key wraparound test: alternate push_front/push_back and pop_front/pop_back
// so that front_ walks both directions around the buffer without growth,
// and confirm order is preserved throughout.
TEST(DequeTest, WraparoundBothEndsPreservesOrder) {
    Deque<int> d;
    d.push_back(2);
    d.push_back(3);
    d.push_front(1);
    d.push_front(0);
    // d: [0, 1, 2, 3]
    std::size_t cap = d.capacity();
    ASSERT_GE(cap, 4u);

    EXPECT_EQ(d.front(), 0);
    d.pop_front();  // front_ advances, wrapping if it was at the last slot
    d.push_back(4);
    // d: [1, 2, 3, 4]
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 4);

    d.pop_back();
    d.push_front(-1);
    // d: [-1, 1, 2, 3]
    EXPECT_EQ(d.front(), -1);
    EXPECT_EQ(d.back(), 3);
    EXPECT_EQ(d.capacity(), cap);
    EXPECT_EQ(d.size(), 4u);
}

TEST(DequeTest, PushFrontOnEmptyBuffer) {
    Deque<int> d;
    d.push_front(1);
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 1);
    d.push_front(0);
    EXPECT_EQ(d.front(), 0);
    EXPECT_EQ(d.back(), 1);
}

TEST(DequeTest, CapacityGrowsWhenNeeded) {
    Deque<int> d;
    std::size_t last_capacity = d.capacity();
    bool grew = false;
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            d.push_back(i);
        } else {
            d.push_front(-i);
        }
        if (d.capacity() > last_capacity) {
            grew = true;
            last_capacity = d.capacity();
        }
    }
    EXPECT_TRUE(grew);
    EXPECT_EQ(d.size(), 100u);
}

TEST(DequeTest, CopyConstructorDeepCopies) {
    Deque<int> a;
    a.push_back(1);
    a.push_back(2);
    Deque<int> b(a);
    b.push_back(3);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.back(), 3);
    EXPECT_EQ(a.back(), 2);
}

TEST(DequeTest, MoveConstructorStealsBuffer) {
    Deque<std::string> a;
    a.push_back("hello");
    a.push_front("world");
    Deque<std::string> b(std::move(a));
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.front(), "world");
    EXPECT_EQ(b.back(), "hello");
    EXPECT_TRUE(a.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(DequeTest, MoveAssignmentStealsBuffer) {
    Deque<std::string> a;
    a.push_back("hello");
    Deque<std::string> b;
    b.push_back("temp");
    b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(b.front(), "hello");
}

TEST(DequeTest, SelfAssignmentIsSafe) {
    Deque<int> a;
    a.push_back(1);
    a.push_back(2);
    a = a;
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(a.front(), 1);
    EXPECT_EQ(a.back(), 2);
}

TEST(DequeTest, WorksAsBaseCollectionPointer) {
    Deque<int> d;
    dsa::Collection<int>* c = &d;
    c->clear();
    EXPECT_TRUE(c->empty());
}
