#include "dsa/linear/queue.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using dsa::Queue;

TEST(QueueTest, StartsEmpty) {
    Queue<int> q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0u);
}

TEST(QueueTest, EnqueueIncreasesSize) {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    EXPECT_EQ(q.size(), 3u);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 3);
}

TEST(QueueTest, DequeueFollowsFifoOrder) {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    EXPECT_EQ(q.front(), 1);
    q.dequeue();
    EXPECT_EQ(q.front(), 2);
    q.dequeue();
    EXPECT_EQ(q.front(), 3);
    q.dequeue();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, FrontThrowsOnEmpty) {
    Queue<int> q;
    EXPECT_THROW(q.front(), std::out_of_range);
}

TEST(QueueTest, BackThrowsOnEmpty) {
    Queue<int> q;
    EXPECT_THROW(q.back(), std::out_of_range);
}

TEST(QueueTest, DequeueThrowsOnEmpty) {
    Queue<int> q;
    EXPECT_THROW(q.dequeue(), std::out_of_range);
}

TEST(QueueTest, ClearEmptiesButKeepsCapacity) {
    Queue<int> q;
    for (int i = 0; i < 10; ++i) {
        q.enqueue(i);
    }
    std::size_t cap_before = q.capacity();
    q.clear();
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.capacity(), cap_before);
    EXPECT_THROW(q.front(), std::out_of_range);
}

// This is the key test for a ring buffer: repeatedly enqueue/dequeue so that
// front_ wraps past the end of the buffer without triggering growth, and
// confirm elements are still delivered in FIFO order and capacity is reused
// instead of growing.
TEST(QueueTest, WraparoundKeepsFifoOrderWithoutGrowing) {
    Queue<int> q;
    for (int i = 0; i < 4; ++i) {
        q.enqueue(i);
    }
    std::size_t cap = q.capacity();
    ASSERT_GE(cap, 4u);

    int next_value = 4;
    for (int round = 0; round < 5; ++round) {
        int expected_front = q.front();
        q.dequeue();
        EXPECT_EQ(expected_front, round);
        q.enqueue(next_value++);
    }
    // Reusing freed slots at the head must not have required growth.
    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.size(), 4u);
}

TEST(QueueTest, WraparoundPreservesOrderStrict) {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    q.enqueue(4);
    std::size_t cap = q.capacity();
    q.dequeue();  // remove 1
    q.dequeue();  // remove 2
    q.enqueue(5);
    q.enqueue(6);  // these should wrap into freed slots if capacity allows
    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.front(), 3);
    q.dequeue();
    EXPECT_EQ(q.front(), 4);
    q.dequeue();
    EXPECT_EQ(q.front(), 5);
    q.dequeue();
    EXPECT_EQ(q.front(), 6);
    q.dequeue();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, CapacityGrowsWhenNeeded) {
    Queue<int> q;
    std::size_t last_capacity = q.capacity();
    bool grew = false;
    for (int i = 0; i < 100; ++i) {
        q.enqueue(i);
        if (q.capacity() > last_capacity) {
            grew = true;
            last_capacity = q.capacity();
        }
    }
    EXPECT_TRUE(grew);
    EXPECT_EQ(q.size(), 100u);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(q.front(), i);
        q.dequeue();
    }
}

TEST(QueueTest, CopyConstructorDeepCopies) {
    Queue<int> a;
    a.enqueue(1);
    a.enqueue(2);
    Queue<int> b(a);
    b.enqueue(3);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 3u);
    a.dequeue();
    EXPECT_EQ(a.front(), 2);
    EXPECT_EQ(b.front(), 1);
}

TEST(QueueTest, MoveConstructorStealsBuffer) {
    Queue<std::string> a;
    a.enqueue("hello");
    a.enqueue("world");
    Queue<std::string> b(std::move(a));
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.front(), "hello");
    EXPECT_TRUE(a.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(QueueTest, MoveAssignmentStealsBuffer) {
    Queue<std::string> a;
    a.enqueue("hello");
    Queue<std::string> b;
    b.enqueue("temp");
    b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(b.front(), "hello");
}

TEST(QueueTest, SelfAssignmentIsSafe) {
    Queue<int> a;
    a.enqueue(1);
    a.enqueue(2);
    a = a;
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(a.front(), 1);
}

TEST(QueueTest, WorksAsBaseCollectionPointer) {
    Queue<int> q;
    dsa::Collection<int>* c = &q;
    c->clear();
    EXPECT_TRUE(c->empty());
}
