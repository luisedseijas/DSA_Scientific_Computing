#include "dsa/trees/binary_tree.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using dsa::BinaryTree;

TEST(BinaryTreeTest, StartsEmpty) {
    BinaryTree<int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), -1);
}

TEST(BinaryTreeTest, InsertLevelOrderFillsBreadthFirst) {
    BinaryTree<int> tree;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) tree.insert(v);
    EXPECT_EQ(tree.size(), 7u);
    // A complete tree of 7 nodes filled level by level.
    EXPECT_EQ(tree.level_order(), (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}

TEST(BinaryTreeTest, HeightSingleNode) {
    BinaryTree<int> tree;
    tree.insert(42);
    EXPECT_EQ(tree.height(), 0);
}

TEST(BinaryTreeTest, HeightBalancedSevenNodes) {
    BinaryTree<int> tree;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) tree.insert(v);
    // Complete tree with 7 nodes: perfectly balanced, height 2.
    EXPECT_EQ(tree.height(), 2);
}

TEST(BinaryTreeTest, TraversalsOnKnownShape) {
    // Level-order insertion of 1..7 builds:
    //             1
    //           /   \
    //          2     3
    //         / \   / \
    //        4   5 6   7
    BinaryTree<int> tree;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) tree.insert(v);

    EXPECT_EQ(tree.preorder(), (std::vector<int>{1, 2, 4, 5, 3, 6, 7}));
    EXPECT_EQ(tree.inorder(), (std::vector<int>{4, 2, 5, 1, 6, 3, 7}));
    EXPECT_EQ(tree.postorder(), (std::vector<int>{4, 5, 2, 6, 7, 3, 1}));
    EXPECT_EQ(tree.level_order(), (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}

TEST(BinaryTreeTest, VisitorCallbackOverloadsMatchVectorOverloads) {
    BinaryTree<int> tree;
    for (int v : {1, 2, 3, 4, 5}) tree.insert(v);

    std::vector<int> visited;
    tree.inorder([&visited](const int& v) { visited.push_back(v); });
    EXPECT_EQ(visited, tree.inorder());
}

TEST(BinaryTreeTest, ClearEmptiesTree) {
    BinaryTree<int> tree;
    for (int v : {1, 2, 3}) tree.insert(v);
    tree.clear();
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), -1);
    EXPECT_TRUE(tree.inorder().empty());
}

TEST(BinaryTreeTest, CopyConstructorDeepCopies) {
    BinaryTree<int> original;
    for (int v : {1, 2, 3, 4, 5}) original.insert(v);

    BinaryTree<int> copy(original);
    EXPECT_EQ(copy.inorder(), original.inorder());

    copy.insert(6);
    EXPECT_NE(copy.size(), original.size());
}

TEST(BinaryTreeTest, CopyAssignmentDeepCopies) {
    BinaryTree<int> original;
    for (int v : {10, 20, 30}) original.insert(v);

    BinaryTree<int> other;
    other.insert(99);
    other = original;

    EXPECT_EQ(other.inorder(), original.inorder());
    other.clear();
    EXPECT_FALSE(original.empty());
}

TEST(BinaryTreeTest, MoveConstructorTransfersOwnership) {
    BinaryTree<int> original;
    for (int v : {1, 2, 3}) original.insert(v);

    BinaryTree<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(BinaryTreeTest, MoveAssignmentTransfersOwnership) {
    BinaryTree<int> original;
    for (int v : {1, 2, 3}) original.insert(v);

    BinaryTree<int> other;
    other.insert(99);
    other = std::move(original);

    EXPECT_EQ(other.size(), 3u);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(BinaryTreeTest, WorksWithNonTrivialType) {
    BinaryTree<std::string> tree;
    tree.insert(std::string("root"));
    tree.insert(std::string("left"));
    tree.insert(std::string("right"));
    EXPECT_EQ(tree.size(), 3u);
    EXPECT_EQ(tree.level_order(), (std::vector<std::string>{"root", "left", "right"}));
}
