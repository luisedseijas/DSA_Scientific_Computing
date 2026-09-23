#include "dsa/linear/stack.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using dsa::Stack;

TEST(StackTest, StartsEmpty) {
    Stack<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

TEST(StackTest, PushIncreasesSize) {
    Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.top(), 3);
}

TEST(StackTest, PopFollowsLifoOrder) {
    Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    EXPECT_EQ(s.top(), 3);
    s.pop();
    EXPECT_EQ(s.top(), 2);
    s.pop();
    EXPECT_EQ(s.top(), 1);
    s.pop();
    EXPECT_TRUE(s.empty());
}

TEST(StackTest, TopThrowsOnEmpty) {
    Stack<int> s;
    EXPECT_THROW(s.top(), std::out_of_range);
}

TEST(StackTest, ConstTopThrowsOnEmpty) {
    const Stack<int> s;
    EXPECT_THROW(s.top(), std::out_of_range);
}

TEST(StackTest, PopThrowsOnEmpty) {
    Stack<int> s;
    EXPECT_THROW(s.pop(), std::out_of_range);
}

TEST(StackTest, ClearEmptiesButKeepsCapacity) {
    Stack<int> s;
    for (int i = 0; i < 10; ++i) {
        s.push(i);
    }
    std::size_t cap_before = s.capacity();
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.capacity(), cap_before);
    EXPECT_THROW(s.top(), std::out_of_range);
}

TEST(StackTest, CapacityGrowsAsElementsArePushed) {
    Stack<int> s;
    std::size_t last_capacity = s.capacity();
    bool grew = false;
    for (int i = 0; i < 100; ++i) {
        s.push(i);
        if (s.capacity() > last_capacity) {
            grew = true;
            last_capacity = s.capacity();
        }
    }
    EXPECT_TRUE(grew);
    EXPECT_EQ(s.size(), 100u);
    // Verify LIFO order is preserved through growth.
    for (int i = 99; i >= 0; --i) {
        EXPECT_EQ(s.top(), i);
        s.pop();
    }
}

TEST(StackTest, CopyConstructorDeepCopies) {
    Stack<int> a;
    a.push(1);
    a.push(2);
    Stack<int> b(a);
    b.push(3);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(a.top(), 2);
    EXPECT_EQ(b.top(), 3);
}

TEST(StackTest, CopyAssignmentDeepCopies) {
    Stack<int> a;
    a.push(1);
    a.push(2);
    Stack<int> b;
    b.push(99);
    b = a;
    a.push(3);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.top(), 2);
    EXPECT_EQ(a.top(), 3);
}

TEST(StackTest, MoveConstructorStealsBuffer) {
    Stack<std::string> a;
    a.push("hello");
    a.push("world");
    Stack<std::string> b(std::move(a));
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.top(), "world");
    EXPECT_TRUE(a.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(StackTest, MoveAssignmentStealsBuffer) {
    Stack<std::string> a;
    a.push("hello");
    Stack<std::string> b;
    b.push("temp");
    b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(b.top(), "hello");
}

TEST(StackTest, SelfAssignmentIsSafe) {
    Stack<int> a;
    a.push(1);
    a.push(2);
    a = a;
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(a.top(), 2);
}

TEST(StackTest, WorksAsBaseCollectionPointer) {
    Stack<int> s;
    dsa::Collection<int>* c = &s;
    c->clear();
    EXPECT_TRUE(c->empty());
}
