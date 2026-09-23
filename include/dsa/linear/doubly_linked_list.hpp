#ifndef DSA_LINEAR_DOUBLY_LINKED_LIST_HPP
#define DSA_LINEAR_DOUBLY_LINKED_LIST_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Lista doblemente enlazada con push/pop O(1) en ambos extremos.
 *
 * Cada nodo mantiene un puntero `prev` y uno `next`, que es lo que
 * permite pop_back en O(1) (a diferencia de SinglyLinkedList, donde
 * encontrar la nueva cola requiere un recorrido O(n)). El puntero
 * extra por nodo es el clásico compromiso espacio/tiempo que los
 * estudiantes deben poder identificar.
 *
 * @tparam T Tipo de elemento almacenado por valor en cada nodo.
 */
template <typename T>
class DoublyLinkedList : public Collection<T> {
   private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        explicit Node(const T& value, Node* prev_ = nullptr, Node* next_ = nullptr)
            : data(value), prev(prev_), next(next_) {}
        explicit Node(T&& value, Node* prev_ = nullptr, Node* next_ = nullptr)
            : data(std::move(value)), prev(prev_), next(next_) {}
    };

    // Implementación compartida para los iteradores bidireccionales mutable y const.
    template <typename ValueType, typename NodeType>
    class IteratorBase {
       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        explicit IteratorBase(NodeType* node = nullptr) : node_(node) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }

        IteratorBase& operator++() {
            node_ = node_->next;
            return *this;
        }

        IteratorBase operator++(int) {
            IteratorBase tmp = *this;
            ++(*this);
            return tmp;
        }

        IteratorBase& operator--() {
            node_ = node_->prev;
            return *this;
        }

        IteratorBase operator--(int) {
            IteratorBase tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const IteratorBase& other) const { return node_ == other.node_; }
        bool operator!=(const IteratorBase& other) const { return node_ != other.node_; }

        NodeType* node_;
    };

   public:
    using Iterator = IteratorBase<T, Node>;
    using ConstIterator = IteratorBase<const T, const Node>;

    /** @brief Iterador inverso construido sobre el Iterator bidireccional. */
    class ReverseIterator {
       public:
        explicit ReverseIterator(Node* node = nullptr) : node_(node) {}

        T& operator*() const { return node_->data; }
        T* operator->() const { return &node_->data; }

        ReverseIterator& operator++() {
            node_ = node_->prev;
            return *this;
        }

        ReverseIterator operator++(int) {
            ReverseIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ReverseIterator& other) const { return node_ == other.node_; }
        bool operator!=(const ReverseIterator& other) const { return node_ != other.node_; }

       private:
        Node* node_;
    };

    DoublyLinkedList() = default;

    DoublyLinkedList(std::initializer_list<T> values) {
        // Construido directamente con punteros crudos (no con push_back) para que
        // este constructor siga funcionando incluso antes de implementar push_back.
        for (const auto& value : values) {
            Node* node = new Node(value, tail_, nullptr);
            if (tail_ != nullptr) {
                tail_->next = node;
            } else {
                head_ = node;
            }
            tail_ = node;
            ++size_;
        }
    }

    DoublyLinkedList(const DoublyLinkedList& other) { copy_from(other); }

    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) {
            DoublyLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~DoublyLinkedList() override { clear(); }

    // --- Collection interface ---

    std::size_t size() const noexcept override { return size_; }

    void clear() override {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    // --- Modifiers ---

    void push_front(const T& value) {
        // TODO(estudiante): crear un nuevo nodo con `value` enlazado antes de head_ (actualizar
        // prev/next) y actualizar head_/tail_/size_.
        (void)value;
        throw std::logic_error("DoublyLinkedList::push_front: no implementado");
    }

    void push_back(const T& value) {
        // TODO(estudiante): crear un nuevo nodo con `value` enlazado después de tail_ (actualizar
        // prev/next) y actualizar head_/tail_/size_.
        (void)value;
        throw std::logic_error("DoublyLinkedList::push_back: no implementado");
    }

    void pop_front() {
        // TODO(estudiante): eliminar el nodo head_ actual, liberar su memoria y actualizar
        // head_/tail_/size_ (incluyendo el nuevo prev de head_).
        throw std::logic_error("DoublyLinkedList::pop_front: no implementado");
    }

    void pop_back() {
        // TODO(estudiante): eliminar el nodo tail_ actual, liberar su memoria y actualizar
        // head_/tail_/size_ (incluyendo el nuevo next de tail_).
        throw std::logic_error("DoublyLinkedList::pop_back: no implementado");
    }

    /** @brief Inserta `value` de modo que quede en la posición `index`. */
    void insert_at(std::size_t index, const T& value) {
        // TODO(estudiante): insertar `value` de modo que quede en la posición `index`, actualizando
        // prev/next de los nodos vecinos (casos especiales: index == 0 e index == size_).
        (void)index;
        (void)value;
        throw std::logic_error("DoublyLinkedList::insert_at: no implementado");
    }

    /** @brief Elimina el elemento ubicado en `index`. */
    void erase_at(std::size_t index) {
        // TODO(estudiante): eliminar el nodo en la posición `index`, reenlazando prev/next de sus
        // vecinos y actualizando head_/tail_/size_ si corresponde.
        (void)index;
        throw std::logic_error("DoublyLinkedList::erase_at: no implementado");
    }

    // --- Accessors ---

    T& front() {
        if (head_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::front: la lista está vacía");
        }
        return head_->data;
    }

    const T& front() const {
        if (head_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::front: la lista está vacía");
        }
        return head_->data;
    }

    T& back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::back: la lista está vacía");
        }
        return tail_->data;
    }

    const T& back() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::back: la lista está vacía");
        }
        return tail_->data;
    }

    Iterator find(const T& value) {
        for (Iterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    ConstIterator find(const T& value) const {
        for (ConstIterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    // --- Iterators ---

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head_); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head_); }
    ConstIterator cend() const { return ConstIterator(nullptr); }

    ReverseIterator rbegin() { return ReverseIterator(tail_); }
    ReverseIterator rend() { return ReverseIterator(nullptr); }

    void swap(DoublyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

   private:
    Node* node_at(std::size_t index) const {
        // Recorre desde el extremo más cercano para reducir el recorrido a la mitad.
        if (index <= size_ / 2) {
            Node* current = head_;
            for (std::size_t i = 0; i < index; ++i) {
                current = current->next;
            }
            return current;
        }
        Node* current = tail_;
        for (std::size_t i = size_ - 1; i > index; --i) {
            current = current->prev;
        }
        return current;
    }

    void copy_from(const DoublyLinkedList& other) {
        // Construido directamente con punteros crudos (no con push_back) para que
        // copiar/mover sigan funcionando incluso antes de implementar push_back.
        for (const auto& value : other) {
            Node* node = new Node(value, tail_, nullptr);
            if (tail_ != nullptr) {
                tail_->next = node;
            } else {
                head_ = node;
            }
            tail_ = node;
            ++size_;
        }
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace dsa

#endif  // DSA_LINEAR_DOUBLY_LINKED_LIST_HPP
