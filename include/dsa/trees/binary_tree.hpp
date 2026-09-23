#ifndef DSA_TREES_BINARY_TREE_HPP
#define DSA_TREES_BINARY_TREE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Un arbol binario generico (no necesariamente ordenado).
 *
 * BinaryTree almacena sus elementos en una estructura enlazada de nodos con
 * hijos `left` y `right`. A diferencia de un BinarySearchTree, no se impone
 * ningun invariante de orden: `insert` siempre coloca el nuevo elemento en
 * el primer espacio libre encontrado en orden por niveles (breadth-first),
 * exactamente como crece un binary heap por posicion. Esto le da a los
 * estudiantes una forma concreta y determinista para razonar (siempre un
 * arbol binario "completo", llenado de izquierda a derecha, nivel por
 * nivel) antes de introducir el invariante de orden BST en el siguiente
 * modulo.
 *
 * Los cuatro recorridos clasicos se exponen tanto como funciones que
 * devuelven un `std::vector<T>` como sobrecargas que aceptan un callback
 * visitante, para que los estudiantes vean ambos estilos usados en la
 * practica.
 *
 * @tparam T Tipo de elemento. Debe ser copiable o movible en construccion.
 */
template <typename T>
class BinaryTree : public Collection<T> {
   private:
    /** @brief Nodo interno: posee sus hijos mediante punteros crudos. */
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

    /** @brief Construye un BinaryTree vacio. */
    BinaryTree() noexcept : root_(nullptr), size_(0) {}

    /** @brief Constructor de copia. Realiza una copia profunda de los nodos de `other`. */
    BinaryTree(const BinaryTree& other) : root_(nullptr), size_(0) {
        root_ = clone_subtree(other.root_);
        size_ = other.size_;
    }

    /** @brief Asignacion de copia. Realiza una copia profunda de los nodos de `other`. */
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            BinaryTree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Constructor de movimiento. Deja `other` vacio. */
    BinaryTree(BinaryTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    /** @brief Asignacion de movimiento. Deja `other` vacio. */
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

    /** @brief Destructor. Libera todos los nodos del arbol. */
    ~BinaryTree() override { destroy_subtree(root_); }

    /** @brief Intercambia el contenido con `other` en tiempo constante. */
    void swap(BinaryTree& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
    }

    /**
     * @brief Inserta `value` en el primer espacio libre en orden por
     * niveles (BFS), exactamente como crece un binary heap por posicion.
     * O(n) porque hay que recorrer la frontera del arbol para hallar ese
     * espacio.
     */
    void insert(const T& value) { insert_impl(Node(value)); }

    /** @copydoc insert(const T&) */
    void insert(T&& value) { insert_impl(Node(std::move(value))); }

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
     * largo. Un arbol vacio tiene altura -1 por convencion, un arbol de un
     * solo nodo tiene altura 0.
     */
    int height() const { return height_of(root_); }

    /** @brief Recorrido en orden (left, node, right) como vector. */
    std::vector<T> inorder() const {
        std::vector<T> out;
        out.reserve(size_);
        inorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido en orden (left, node, right) invocando `visit` por cada elemento. */
    void inorder(const VisitFn& visit) const { inorder_impl(root_, visit); }

    /** @brief Recorrido pre-orden (node, left, right) como vector. */
    std::vector<T> preorder() const {
        std::vector<T> out;
        out.reserve(size_);
        preorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido pre-orden (node, left, right) invocando `visit` por cada elemento. */
    void preorder(const VisitFn& visit) const { preorder_impl(root_, visit); }

    /** @brief Recorrido post-orden (left, right, node) como vector. */
    std::vector<T> postorder() const {
        std::vector<T> out;
        out.reserve(size_);
        postorder([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido post-orden (left, right, node) invocando `visit` por cada elemento. */
    void postorder(const VisitFn& visit) const { postorder_impl(root_, visit); }

    /** @brief Recorrido por niveles (breadth-first) como vector. */
    std::vector<T> level_order() const {
        std::vector<T> out;
        out.reserve(size_);
        level_order([&out](const T& v) { out.push_back(v); });
        return out;
    }

    /** @brief Recorrido por niveles (breadth-first) invocando `visit` por cada elemento. */
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
    // Expuesto a clases derivadas (p. ej. un futuro modulo AVL/afin a BST)
    // que quieran reutilizar el tipo de nodo y las funciones auxiliares de
    // recorrido.
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
        (void)fresh;
        // TODO(estudiante): insertar `fresh.value` en el primer espacio libre
        // recorriendo el arbol en orden por niveles (BFS con una cola),
        // exactamente como crece un binary heap por posicion; recordar
        // incrementar `size_` al insertar.
        throw std::logic_error("BinaryTree::insert: no implementado");
    }
};

}  // namespace dsa

#endif  // DSA_TREES_BINARY_TREE_HPP
