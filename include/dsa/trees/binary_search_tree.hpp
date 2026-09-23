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
 * @brief Un arbol binario de busqueda (BST) ordenado con `operator<`.
 *
 * Cada nodo mantiene el invariante BST: todo en su subarbol izquierdo
 * compara menor que el valor del nodo, y todo en su subarbol derecho
 * compara mayor. Los duplicados (valores que comparan iguales en ambas
 * direcciones) son rechazados por `insert`, reflejando la semantica de
 * `std::set`.
 *
 * Los nodos guardan un puntero `parent` para que un const_iterator en
 * orden pueda recorrer el arbol en O(1) amortizado por paso sin una pila
 * auxiliar, el mismo truco que usan los iteradores de `std::map`/`std::set`.
 *
 * @tparam T Tipo de elemento. Debe estar totalmente ordenado por
 * `operator<` y ser copiable o movible en construccion.
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

    /** @brief Iterador de avance, de solo lectura, en orden. */
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

    /** @brief Construye un BinarySearchTree vacio. */
    BinarySearchTree() noexcept : root_(nullptr), size_(0) {}

    /** @brief Constructor de copia. Realiza una copia profunda y estructural de `other`. */
    BinarySearchTree(const BinarySearchTree& other) : root_(nullptr), size_(0) {
        root_ = clone_subtree(other.root_, nullptr);
        size_ = other.size_;
    }

    /** @brief Asignacion de copia. Realiza una copia profunda y estructural de `other`. */
    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            BinarySearchTree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Constructor de movimiento. Deja `other` vacio. */
    BinarySearchTree(BinarySearchTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    /** @brief Asignacion de movimiento. Deja `other` vacio. */
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

    /** @brief Destructor. Libera todos los nodos del arbol. */
    ~BinarySearchTree() override { destroy_subtree(root_); }

    /** @brief Intercambia el contenido con `other` en tiempo constante. */
    void swap(BinarySearchTree& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
    }

    /**
     * @brief Inserta `value`, manteniendo el invariante BST. Los duplicados
     * se ignoran (devuelve false). O(h) donde h es la altura actual.
     */
    bool insert(const T& value) { return insert_impl(value); }

    /** @copydoc insert(const T&) */
    bool insert(T&& value) { return insert_impl(std::move(value)); }

    /**
     * @brief Elimina `value` si esta presente, manejando los tres casos
     * clasicos de borrado (hoja, un solo hijo, dos hijos via sucesor
     * in-order). O(h). Devuelve true si se elimino un elemento.
     */
    bool erase(const T& value) {
        Node* target = find_node(value);
        if (target == nullptr) return false;
        erase_node(target);
        --size_;
        return true;
    }

    /** @brief Devuelve true si `value` esta presente en el arbol. O(h). */
    bool contains(const T& value) const { return find_node(value) != nullptr; }

    /** @brief Alias de `contains`, siguiendo la terminologia comun de ADT. */
    bool find(const T& value) const { return contains(value); }

    /** @brief Elemento minimo. Lanza std::out_of_range si el arbol esta vacio. */
    const T& min() const {
        if (root_ == nullptr) throw std::out_of_range("BinarySearchTree::min en arbol vacio");
        return leftmost(root_)->value;
    }

    /** @brief Elemento maximo. Lanza std::out_of_range si el arbol esta vacio. */
    const T& max() const {
        if (root_ == nullptr) throw std::out_of_range("BinarySearchTree::max en arbol vacio");
        return rightmost(root_)->value;
    }

    /** @brief Numero de elementos almacenados actualmente. */
    std::size_t size() const noexcept override { return size_; }

    /** @brief Elimina todos los elementos, dejando el arbol vacio. */
    void clear() override {
        destroy_subtree(root_);
        root_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief Altura del arbol: numero de aristas en el camino raiz-hoja mas
     * largo. Un arbol vacio tiene altura -1, un arbol de un solo nodo tiene
     * altura 0.
     */
    int height() const { return height_of(root_); }

    /** @brief Recorrido en orden como vector. Siempre ordenado ascendente. */
    std::vector<T> inorder() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido en orden invocando `visit` por cada elemento, orden ascendente. */
    void inorder(const VisitFn& visit) const { inorder_impl(root_, visit); }

    /** @brief Recorrido pre-orden (node, left, right) como vector. */
    std::vector<T> preorder() const {
        std::vector<T> out;
        out.reserve(size_);
        preorder_impl(root_, [&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido post-orden (left, right, node) como vector. */
    std::vector<T> postorder() const {
        std::vector<T> out;
        out.reserve(size_);
        postorder_impl(root_, [&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido por niveles (breadth-first) como vector. */
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

    /** @brief Iterador al elemento minimo (o end() si esta vacio). O(h). */
    const_iterator begin() const {
        return const_iterator(root_ == nullptr ? nullptr : leftmost(root_));
    }

    /** @brief Iterador de fin (past-the-end). */
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

    // Reemplaza el subarbol con raiz en `node` por el subarbol con raiz en
    // `replacement` desde la perspectiva del padre de node (no toca los
    // hijos propios de `replacement`). Funcion auxiliar "transplant"
    // estandar usada por los 3 casos de erase.
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
