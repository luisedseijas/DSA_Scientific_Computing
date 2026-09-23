#ifndef DSA_LINEAR_SINGLY_LINKED_LIST_HPP
#define DSA_LINEAR_SINGLY_LINKED_LIST_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Singly linked list with O(1) push/pop at both ends.
 *
 * Each node only knows its successor. A tail pointer is kept so that
 * push_back/pop_front stay O(1); pop_back, however, is O(n) because
 * finding the new tail requires walking from head (there is no way to
 * go "backwards" in a singly linked structure). This trade-off is the
 * whole pedagogical point of contrasting this class with
 * DoublyLinkedList, where pop_back is O(1).
 *
 * @tparam T Element type stored by value in each node.
 */
template <typename T>
class SinglyLinkedList : public Collection<T> {
   private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value, Node* next_ = nullptr) : data(value), next(next_) {}
        explicit Node(T&& value, Node* next_ = nullptr) : data(std::move(value)), next(next_) {}
    };

   public:
    /** @brief Forward iterator over the list's elements. */
    class Iterator {
       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node = nullptr) : node_(node) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }

        Iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }

       private:
        Node* node_;
        friend class SinglyLinkedList;
    };

    /** @brief Read-only forward iterator. */
    class ConstIterator {
       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        explicit ConstIterator(const Node* node = nullptr) : node_(node) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }

        ConstIterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const { return node_ == other.node_; }
        bool operator!=(const ConstIterator& other) const { return node_ != other.node_; }

       private:
        const Node* node_;
    };

    SinglyLinkedList() = default;

    SinglyLinkedList(std::initializer_list<T> values) {
        // Built directly with raw pointers (not push_back) so this constructor
        // keeps working even before push_back is implemented.
        for (const auto& value : values) {
            Node* node = new Node(value);
            if (tail_ == nullptr) {
                head_ = tail_ = node;
            } else {
                tail_->next = node;
                tail_ = node;
            }
            ++size_;
        }
    }

    /** @brief Deep copy constructor. */
    SinglyLinkedList(const SinglyLinkedList& other) { copy_from(other); }

    /** @brief Copy assignment (copy-and-swap). */
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    /** @brief Move constructor leaves `other` empty. */
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    /** @brief Move assignment. */
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
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

    ~SinglyLinkedList() override { clear(); }

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
        // TODO(estudiante): crear un nuevo nodo con `value` enlazado al head_ actual y actualizar head_/tail_/size_.
        (void)value;
        throw std::logic_error("SinglyLinkedList::push_front: no implementado");
    }

    void push_front(T&& value) {
        // TODO(estudiante): crear un nuevo nodo moviendo `value`, enlazarlo al head_ actual y actualizar head_/tail_/size_.
        (void)value;
        throw std::logic_error("SinglyLinkedList::push_front: no implementado");
    }

    void push_back(const T& value) {
        // TODO(estudiante): crear un nuevo nodo con `value` y enlazarlo después de tail_ (o como único nodo si la lista está vacía).
        (void)value;
        throw std::logic_error("SinglyLinkedList::push_back: no implementado");
    }

    void push_back(T&& value) {
        // TODO(estudiante): crear un nuevo nodo moviendo `value` y enlazarlo después de tail_ (o como único nodo si la lista está vacía).
        (void)value;
        throw std::logic_error("SinglyLinkedList::push_back: no implementado");
    }

    void pop_front() {
        // TODO(estudiante): eliminar el nodo head_ actual, liberar su memoria y actualizar head_/tail_/size_.
        throw std::logic_error("SinglyLinkedList::pop_front: no implementado");
    }

    /** @brief Removes the last element. O(n): requires finding the new tail. */
    void pop_back() {
        // TODO(estudiante): encontrar el nodo previo a tail_ recorriendo desde head_, eliminar tail_ y actualizar tail_/size_.
        throw std::logic_error("SinglyLinkedList::pop_back: no implementado");
    }

    /** @brief Inserts `value` right after the node currently at `index`. */
    void insert_after(std::size_t index, const T& value) {
        // TODO(estudiante): insertar un nuevo nodo con `value` inmediatamente después del nodo en `index`, actualizando tail_ si corresponde.
        (void)index;
        (void)value;
        throw std::logic_error("SinglyLinkedList::insert_after: no implementado");
    }

    /** @brief Inserts `value` so it becomes the element at `index`. */
    void insert_at(std::size_t index, const T& value) {
        // TODO(estudiante): insertar `value` de modo que quede en la posición `index` (casos especiales: index == 0 e index == size_).
        (void)index;
        (void)value;
        throw std::logic_error("SinglyLinkedList::insert_at: no implementado");
    }

    /** @brief Removes the element located at `index`. */
    void erase_at(std::size_t index) {
        // TODO(estudiante): eliminar el nodo en la posición `index`, reenlazando el nodo previo y actualizando tail_/size_ si corresponde.
        (void)index;
        throw std::logic_error("SinglyLinkedList::erase_at: no implementado");
    }

    // --- Accessors ---

    T& front() {
        if (head_ == nullptr) {
            throw std::out_of_range("SinglyLinkedList::front: list is empty");
        }
        return head_->data;
    }

    const T& front() const {
        if (head_ == nullptr) {
            throw std::out_of_range("SinglyLinkedList::front: list is empty");
        }
        return head_->data;
    }

    T& back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("SinglyLinkedList::back: list is empty");
        }
        return tail_->data;
    }

    const T& back() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("SinglyLinkedList::back: list is empty");
        }
        return tail_->data;
    }

    /** @brief Returns an iterator to the first node equal to `value`, or end(). */
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

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

   private:
    Node* node_at(std::size_t index) const {
        Node* current = head_;
        for (std::size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

    void copy_from(const SinglyLinkedList& other) {
        // Built directly with raw pointers (not push_back) so copy/move keep
        // working even before push_back is implemented.
        for (const auto& value : other) {
            Node* node = new Node(value);
            if (tail_ == nullptr) {
                head_ = tail_ = node;
            } else {
                tail_->next = node;
                tail_ = node;
            }
            ++size_;
        }
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace dsa

#endif  // DSA_LINEAR_SINGLY_LINKED_LIST_HPP
