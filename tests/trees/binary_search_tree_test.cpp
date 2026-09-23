#include "dsa/trees/binary_search_tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

using dsa::BinarySearchTree;

namespace {

BinarySearchTree<int> MakeSampleTree() {
    //               50
    //             /    \
    //           30      70
    //          /  \    /  \
    //        20   40  60   80
    BinarySearchTree<int> tree;
    for (int v : {50, 30, 70, 20, 40, 60, 80}) tree.insert(v);
    return tree;
}

}  // namespace

TEST(BinarySearchTreeTest, StartsEmpty) {
    BinarySearchTree<int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), -1);
}

TEST(BinarySearchTreeTest, FindOnEmptyTreeReturnsFalse) {
    BinarySearchTree<int> tree;
    EXPECT_FALSE(tree.contains(1));
    EXPECT_FALSE(tree.find(1));
    EXPECT_FALSE(tree.erase(1));
}

TEST(BinarySearchTreeTest, InsertRejectsDuplicates) {
    BinarySearchTree<int> tree;
    EXPECT_TRUE(tree.insert(5));
    EXPECT_FALSE(tree.insert(5));
    EXPECT_EQ(tree.size(), 1u);
}

TEST(BinarySearchTreeTest, InorderTraversalIsAlwaysSorted) {
    BinarySearchTree<int> tree;
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 90, 35};
    for (int v : values) tree.insert(v);

    std::vector<int> inorder = tree.inorder();
    std::vector<int> sorted_values = values;
    std::sort(sorted_values.begin(), sorted_values.end());

    EXPECT_EQ(inorder, sorted_values);
    EXPECT_TRUE(std::is_sorted(inorder.begin(), inorder.end()));
}

TEST(BinarySearchTreeTest, ContainsFindsInsertedValues) {
    BinarySearchTree<int> tree = MakeSampleTree();
    for (int v : {50, 30, 70, 20, 40, 60, 80}) EXPECT_TRUE(tree.contains(v));
    EXPECT_FALSE(tree.contains(999));
    EXPECT_FALSE(tree.contains(25));
}

TEST(BinarySearchTreeTest, MinAndMax) {
    BinarySearchTree<int> tree = MakeSampleTree();
    EXPECT_EQ(tree.min(), 20);
    EXPECT_EQ(tree.max(), 80);
}

TEST(BinarySearchTreeTest, MinMaxOnEmptyTreeThrows) {
    BinarySearchTree<int> tree;
    EXPECT_THROW(tree.min(), std::out_of_range);
    EXPECT_THROW(tree.max(), std::out_of_range);
}

TEST(BinarySearchTreeTest, HeightEmptySingleAndBalanced) {
    BinarySearchTree<int> empty_tree;
    EXPECT_EQ(empty_tree.height(), -1);

    BinarySearchTree<int> single;
    single.insert(1);
    EXPECT_EQ(single.height(), 0);

    BinarySearchTree<int> balanced = MakeSampleTree();
    EXPECT_EQ(balanced.height(), 2);
}

TEST(BinarySearchTreeTest, HeightDegenerateTree) {
    // Insertar una secuencia ya ordenada degenera en una lista enlazada.
    BinarySearchTree<int> tree;
    for (int v : {1, 2, 3, 4, 5}) tree.insert(v);
    EXPECT_EQ(tree.height(), 4);  // n - 1 aristas
}

TEST(BinarySearchTreeTest, EraseLeafNode) {
    BinarySearchTree<int> tree = MakeSampleTree();
    EXPECT_TRUE(tree.erase(20));  // 20 es una hoja
    EXPECT_FALSE(tree.contains(20));
    EXPECT_EQ(tree.size(), 6u);
    std::vector<int> after_erase = tree.inorder();
    EXPECT_TRUE(std::is_sorted(after_erase.begin(), after_erase.end()));
}

TEST(BinarySearchTreeTest, EraseNodeWithSingleChild) {
    BinarySearchTree<int> tree = MakeSampleTree();
    tree.erase(20);  // 30 ahora solo tiene el hijo derecho 40
    EXPECT_TRUE(tree.erase(30));
    EXPECT_FALSE(tree.contains(30));
    EXPECT_TRUE(tree.contains(40));
    EXPECT_EQ(tree.size(), 5u);
    std::vector<int> expected = {40, 50, 60, 70, 80};
    EXPECT_EQ(tree.inorder(), expected);
}

TEST(BinarySearchTreeTest, EraseNodeWithTwoChildren) {
    BinarySearchTree<int> tree = MakeSampleTree();
    EXPECT_TRUE(tree.erase(30));  // tiene los hijos 20 y 40
    EXPECT_FALSE(tree.contains(30));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(40));
    EXPECT_EQ(tree.size(), 6u);
    std::vector<int> expected = {20, 40, 50, 60, 70, 80};
    EXPECT_EQ(tree.inorder(), expected);
}

TEST(BinarySearchTreeTest, EraseRootWithTwoChildren) {
    BinarySearchTree<int> tree = MakeSampleTree();
    EXPECT_TRUE(tree.erase(50));
    EXPECT_FALSE(tree.contains(50));
    EXPECT_EQ(tree.size(), 6u);
    std::vector<int> expected = {20, 30, 40, 60, 70, 80};
    std::vector<int> after_erase = tree.inorder();
    EXPECT_EQ(after_erase, expected);
    EXPECT_TRUE(std::is_sorted(after_erase.begin(), after_erase.end()));
}

TEST(BinarySearchTreeTest, EraseAllNodesLeavesEmptyTree) {
    BinarySearchTree<int> tree = MakeSampleTree();
    for (int v : {50, 30, 70, 20, 40, 60, 80}) EXPECT_TRUE(tree.erase(v));
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.height(), -1);
    EXPECT_FALSE(tree.erase(50));
}

TEST(BinarySearchTreeTest, EraseNonExistentValueReturnsFalse) {
    BinarySearchTree<int> tree = MakeSampleTree();
    EXPECT_FALSE(tree.erase(12345));
    EXPECT_EQ(tree.size(), 7u);
}

TEST(BinarySearchTreeTest, InorderIteratorWalksInAscendingOrder) {
    BinarySearchTree<int> tree = MakeSampleTree();
    std::vector<int> via_iterator(tree.begin(), tree.end());
    EXPECT_EQ(via_iterator, tree.inorder());
    EXPECT_TRUE(std::is_sorted(via_iterator.begin(), via_iterator.end()));
}

TEST(BinarySearchTreeTest, IteratorOnEmptyTreeIsBeginEqualsEnd) {
    BinarySearchTree<int> tree;
    EXPECT_EQ(tree.begin(), tree.end());
}

TEST(BinarySearchTreeTest, ClearEmptiesTree) {
    BinarySearchTree<int> tree = MakeSampleTree();
    tree.clear();
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), -1);
}

TEST(BinarySearchTreeTest, CopyConstructorDeepCopies) {
    BinarySearchTree<int> original = MakeSampleTree();
    BinarySearchTree<int> copy(original);
    EXPECT_EQ(copy.inorder(), original.inorder());

    copy.erase(50);
    EXPECT_TRUE(original.contains(50));
    EXPECT_FALSE(copy.contains(50));
}

TEST(BinarySearchTreeTest, CopyAssignmentDeepCopies) {
    BinarySearchTree<int> original = MakeSampleTree();
    BinarySearchTree<int> other;
    other.insert(999);
    other = original;

    EXPECT_EQ(other.inorder(), original.inorder());
    other.clear();
    EXPECT_FALSE(original.empty());
}

TEST(BinarySearchTreeTest, MoveConstructorTransfersOwnership) {
    BinarySearchTree<int> original = MakeSampleTree();
    std::size_t original_size = original.size();

    BinarySearchTree<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), original_size);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(BinarySearchTreeTest, MoveAssignmentTransfersOwnership) {
    BinarySearchTree<int> original = MakeSampleTree();
    std::size_t original_size = original.size();

    BinarySearchTree<int> other;
    other.insert(999);
    other = std::move(original);

    EXPECT_EQ(other.size(), original_size);
    EXPECT_TRUE(original.empty());  // NOLINT(bugprone-use-after-move)
}

TEST(BinarySearchTreeTest, WorksWithNonTrivialType) {
    BinarySearchTree<std::string> tree;
    for (const auto& s : {"banana", "apple", "cherry"}) tree.insert(std::string(s));
    std::vector<std::string> expected = {"apple", "banana", "cherry"};
    EXPECT_EQ(tree.inorder(), expected);
}
