#ifndef DSA_TREES_BINARY_SEARCH_TREE_HPP
#define DSA_TREES_BINARY_SEARCH_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief A binary search tree (BST) ordered with `operator<`.
 *
 * Every node maintains the BST invariant: everything in its left subtree
 * compares less than the node's value, and everything in its right subtree
 * compares greater. Duplicates (values that compare equal in both
 * directions) are rejected by `insert`, mirroring `std::set` semantics.
 *
 * Nodes keep a `parent` pointer so that an in-order const_iterator can walk
 * the tree in O(1) amortized per step without an auxiliary stack, the same
 * trick `std::map`/`std::set` iterators use.
 *
 * @tparam T Element type. Must be totally ordered by `operator<` and copy
 * or move constructible.
 */
template <typename T>
class BinarySearchTree : public Collection<T> {
   private:
    struct Node {
        T value;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        explicit Node(const T& v, Node* p = nullptr) : value(v), parent(p) {}
        explicit Node(T&& v, Node* p = nullptr) : value(std::move(v)), parent(p) {}
    };

   public:
    using value_type = T;
    using VisitFn = std::function<void(const T&)>;

    /** @brief Forward, read-only in-order iterator. */
    class const_iterator {
       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() : node_(nullptr) {}
        explicit const_iterator(Node* node) : node_(node) {}

        reference operator*() const { return node_->value; }
        pointer operator->() const { return &node_->value; }

        const_iterator& operator++() {
            node_ = successor(node_);
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return node_ == other.node_; }
        bool operator!=(const const_iterator& other) const { return node_ != other.node_; }

       private:
        Node* node_;

        static Node* successor(Node* node) {
            if (node == nullptr) return nullptr;
            if (node->right != nullptr) {
                node = node->right;
                while (node->left != nullptr) node = node->left;
                return node;
            }
            Node* parent = node->parent;
            while (parent != nullptr && node == parent->right) {
                node = parent;
                parent = parent->parent;
            }
            return parent;
        }
    };

    /** @brief Constructs an empty BinarySearchTree. */
    BinarySearchTree() noexcept : root_(nullptr), size_(0) {}

    /** @brief Copy constructor. Performs a deep, structural copy of `other`. */
    BinarySearchTree(const BinarySearchTree& other) : root_(nullptr), size_(0) {
        root_ = clone_subtree(other.root_, nullptr);
        size_ = other.size_;
    }

    /** @brief Copy assignment. Performs a deep, structural copy of `other`. */
    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            BinarySearchTree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move constructor. Leaves `other` empty. */
    BinarySearchTree(BinarySearchTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    /** @brief Move assignment. Leaves `other` empty. */
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept {
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
    ~BinarySearchTree() override { destroy_subtree(root_); }

    /** @brief Swaps contents with `other` in constant time. */
    void swap(BinarySearchTree& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
    }

    /**
     * @brief Inserts `value`, keeping the BST invariant. Duplicates are
     * ignored (returns false). O(h) where h is the current height.
     */
    bool insert(const T& value) { return insert_impl(value); }

    /** @copydoc insert(const T&) */
    bool insert(T&& value) { return insert_impl(std::move(value)); }

    /**
     * @brief Removes `value` if present, handling the three classic erase
     * cases (leaf, single child, two children via in-order successor).
     * O(h). Returns true if an element was removed.
     */
    bool erase(const T& value) {
        Node* target = find_node(value);
        if (target == nullptr) return false;
        erase_node(target);
        --size_;
        return true;
    }

    /** @brief Returns true if `value` is present in the tree. O(h). */
    bool contains(const T& value) const { return find_node(value) != nullptr; }

    /** @brief Alias for `contains`, matching common ADT terminology. */
    bool find(const T& value) const { return contains(value); }

    /** @brief Smallest element. Throws std::out_of_range if the tree is empty. */
    const T& min() const {
        if (root_ == nullptr) throw std::out_of_range("BinarySearchTree::min on empty tree");
        return leftmost(root_)->value;
    }

    /** @brief Largest element. Throws std::out_of_range if the tree is empty. */
    const T& max() const {
        if (root_ == nullptr) throw std::out_of_range("BinarySearchTree::max on empty tree");
        return rightmost(root_)->value;
    }

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
     * path. An empty tree has height -1, a single-node tree has height 0.
     */
    int height() const { return height_of(root_); }

    /** @brief In-order traversal as a vector. Always sorted ascending. */
    std::vector<T> inorder() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief In-order traversal invoking `visit` per element, ascending order. */
    void inorder(const VisitFn& visit) const { inorder_impl(root_, visit); }

    /** @brief Pre-order traversal (node, left, right) as a vector. */
    std::vector<T> preorder() const {
        std::vector<T> out;
        out.reserve(size_);
        preorder_impl(root_, [&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Post-order traversal (left, right, node) as a vector. */
    std::vector<T> postorder() const {
        std::vector<T> out;
        out.reserve(size_);
        postorder_impl(root_, [&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Level-order (breadth-first) traversal as a vector. */
    std::vector<T> level_order() const {
        std::vector<T> out;
        out.reserve(size_);
        if (root_ == nullptr) return out;
        std::queue<Node*> pending;
        pending.push(root_);
        while (!pending.empty()) {
            Node* current = pending.front();
            pending.pop();
            out.push_back(current->value);
            if (current->left != nullptr) pending.push(current->left);
            if (current->right != nullptr) pending.push(current->right);
        }
        return out;
    }

    /** @brief Iterator to the smallest element (or end() if empty). O(h). */
    const_iterator begin() const {
        return const_iterator(root_ == nullptr ? nullptr : leftmost(root_));
    }

    /** @brief Past-the-end iterator. */
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

   private:
    Node* root_;
    std::size_t size_;

    static Node* leftmost(Node* node) {
        while (node->left != nullptr) node = node->left;
        return node;
    }

    static Node* rightmost(Node* node) {
        while (node->right != nullptr) node = node->right;
        return node;
    }

    static Node* clone_subtree(Node* node, Node* parent) {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->value, parent);
        copy->left = clone_subtree(node->left, copy);
        copy->right = clone_subtree(node->right, copy);
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

    Node* find_node(const T& value) const {
        (void)value;
        // TODO(estudiante): recorrer el arbol desde la raiz comparando
        // `value` con `current->value` usando `operator<`; bajar a la
        // izquierda si `value` es menor, a la derecha si es mayor, y
        // devolver el nodo cuando ninguna comparacion es cierta (valor
        // encontrado). Devolver nullptr si se llega a un puntero nulo.
        throw std::logic_error("BinarySearchTree::find: no implementado");
    }

    template <typename U>
    bool insert_impl(U&& value) {
        (void)value;
        // TODO(estudiante): insertar `value` manteniendo el invariante BST:
        // descender desde la raiz comparando con `operator<` hasta hallar el
        // puntero nulo donde debe colgar el nuevo nodo (guardando el padre
        // recorrido); si el valor ya existe (ni menor ni mayor), no insertar
        // y devolver false; en otro caso crear el nodo, enlazarlo, aumentar
        // `size_` y devolver true.
        throw std::logic_error("BinarySearchTree::insert: no implementado");
    }

    // Replaces the subtree rooted at `node` with the subtree rooted at
    // `replacement` in node's parent's eyes (does not touch `replacement`'s
    // own children). Standard "transplant" helper used by erase's 3 cases.
    void transplant(Node* node, Node* replacement) {
        if (node->parent == nullptr) {
            root_ = replacement;
        } else if (node == node->parent->left) {
            node->parent->left = replacement;
        } else {
            node->parent->right = replacement;
        }
        if (replacement != nullptr) replacement->parent = node->parent;
    }

    void erase_node(Node* target) {
        (void)target;
        // TODO(estudiante): eliminar `target` manejando los 3 casos clasicos:
        // (1) hoja o solo hijo derecho -> transplant(target, target->right);
        // (2) solo hijo izquierdo -> transplant(target, target->left);
        // (3) dos hijos -> localizar el sucesor in-order (leftmost(target->right)),
        // desconectarlo de su lugar (transplant con su propio hijo derecho si
        // no es hijo directo de target), y transplant(target, successor)
        // enlazando successor->left = target->left. En todos los casos,
        // liberar `target` con delete al terminar.
        throw std::logic_error("BinarySearchTree::erase: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_TREES_BINARY_SEARCH_TREE_HPP
