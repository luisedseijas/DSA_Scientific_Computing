#ifndef DSA_TREES_BINARY_TREE_HPP
#define DSA_TREES_BINARY_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A generic (not necessarily ordered) binary tree.
 *
 * BinaryTree stores its elements in a linked structure of nodes with `left`
 * and `right` children. Unlike a BinarySearchTree, no ordering invariant is
 * enforced: `insert` always places the new element in the first free slot
 * found in level order (breadth-first), which is exactly how you would grow
 * a binary heap by position. This gives students a concrete, deterministic
 * shape to reason about (always a "complete" binary tree, filled left to
 * right, level by level) before introducing the BST ordering invariant in
 * the next module.
 *
 * The four classic traversals are exposed both as functions that return a
 * `std::vector<T>` and as overloads that accept a visitor callback, so
 * students can see both styles used in the wild.
 *
 * @tparam T Element type. Must be copy or move constructible.
 */
template <typename T>
class BinaryTree : public Collection<T> {
   private:
    /** @brief Internal node: owns its children via raw pointers. */
    struct Node {
        T value;
        Node* left = nullptr;
        Node* right = nullptr;

        explicit Node(const T& v) : value(v) {}
        explicit Node(T&& v) : value(std::move(v)) {}
    };

   public:
    using value_type = T;
    using VisitFn = std::function<void(const T&)>;

    /** @brief Constructs an empty BinaryTree. */
    BinaryTree() noexcept : root_(nullptr), size_(0) {}

    /** @brief Copy constructor. Performs a deep copy of `other`'s nodes. */
    BinaryTree(const BinaryTree& other) : root_(nullptr), size_(0) {
        root_ = clone_subtree(other.root_);
        size_ = other.size_;
    }

    /** @brief Copy assignment. Performs a deep copy of `other`'s nodes. */
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            BinaryTree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move constructor. Leaves `other` empty. */
    BinaryTree(BinaryTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    /** @brief Move assignment. Leaves `other` empty. */
    BinaryTree& operator=(BinaryTree&& other) noexcept {
        if (this != &other) {
            destroy_subtree(root_);
            root_ = other.root_;
            size_ = other.size_;
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    /** @brief Destructor. Frees every node in the tree. */
    ~BinaryTree() override { destroy_subtree(root_); }

    /** @brief Swaps contents with `other` in constant time. */
    void swap(BinaryTree& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
    }

    /**
     * @brief Inserts `value` in the first free slot in level order (BFS),
     * exactly as a binary heap grows by position. O(n) because we must walk
     * the frontier of the tree to find that slot.
     */
    void insert(const T& value) { insert_impl(Node(value)); }

    /** @copydoc insert(const T&) */
    void insert(T&& value) { insert_impl(Node(std::move(value))); }

    /** @brief Number of elements currently stored. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Removes every element, leaving the tree empty. */
    void clear() override {
        destroy_subtree(root_);
        root_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief Height of the tree: number of edges on the longest root-to-leaf
     * path. An empty tree has height -1 by convention, a single-node tree has
     * height 0.
     */
    int height() const { return height_of(root_); }

    /** @brief In-order traversal (left, node, right) as a vector. */
    std::vector<T> inorder() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief In-order traversal (left, node, right) invoking `visit` per element. */
    void inorder(const VisitFn& visit) const { inorder_impl(root_, visit); }

    /** @brief Pre-order traversal (node, left, right) as a vector. */
    std::vector<T> preorder() const {
        std::vector<T> out;
        out.reserve(size_);
        preorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Pre-order traversal (node, left, right) invoking `visit` per element. */
    void preorder(const VisitFn& visit) const { preorder_impl(root_, visit); }

    /** @brief Post-order traversal (left, right, node) as a vector. */
    std::vector<T> postorder() const {
        std::vector<T> out;
        out.reserve(size_);
        postorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Post-order traversal (left, right, node) invoking `visit` per element. */
    void postorder(const VisitFn& visit) const { postorder_impl(root_, visit); }

    /** @brief Level-order (breadth-first) traversal as a vector. */
    std::vector<T> level_order() const {
        std::vector<T> out;
        out.reserve(size_);
        level_order([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Level-order (breadth-first) traversal invoking `visit` per element. */
    void level_order(const VisitFn& visit) const {
        if (root_ == nullptr) return;
        std::queue<Node*> pending;
        pending.push(root_);
        while (!pending.empty()) {
            Node* current = pending.front();
            pending.pop();
            visit(current->value);
            if (current->left != nullptr) pending.push(current->left);
            if (current->right != nullptr) pending.push(current->right);
        }
    }

   protected:
    // Exposed to derived classes (e.g. a future AVL/BST-adjacent module) that
    // may want to reuse the node type and traversal helpers.
    Node* root_;
    std::size_t size_;

    static Node* clone_subtree(Node* node) {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->value);
        copy->left = clone_subtree(node->left);
        copy->right = clone_subtree(node->right);
        return copy;
    }

    static void destroy_subtree(Node* node) {
        if (node == nullptr) return;
        destroy_subtree(node->left);
        destroy_subtree(node->right);
        delete node;
    }

    static int height_of(Node* node) {
        if (node == nullptr) return -1;
        return 1 + std::max(height_of(node->left), height_of(node->right));
    }

    static void inorder_impl(Node* node, const VisitFn& visit) {
        if (node == nullptr) return;
        inorder_impl(node->left, visit);
        visit(node->value);
        inorder_impl(node->right, visit);
    }

    static void preorder_impl(Node* node, const VisitFn& visit) {
        if (node == nullptr) return;
        visit(node->value);
        preorder_impl(node->left, visit);
        preorder_impl(node->right, visit);
    }

    static void postorder_impl(Node* node, const VisitFn& visit) {
        if (node == nullptr) return;
        postorder_impl(node->left, visit);
        postorder_impl(node->right, visit);
        visit(node->value);
    }

   private:
    void insert_impl(Node&& fresh) {
        Node* new_node = new Node(std::move(fresh.value));
        if (root_ == nullptr) {
            root_ = new_node;
            ++size_;
            return;
        }
        // BFS to find the first node missing a left or right child.
        std::queue<Node*> pending;
        pending.push(root_);
        while (!pending.empty()) {
            Node* current = pending.front();
            pending.pop();
            if (current->left == nullptr) {
                current->left = new_node;
                ++size_;
                return;
            }
            pending.push(current->left);
            if (current->right == nullptr) {
                current->right = new_node;
                ++size_;
                return;
            }
            pending.push(current->right);
        }
    }
};

}  // namespace dsa

#endif  // DSA_TREES_BINARY_TREE_HPP
