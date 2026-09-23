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
 * @brief Doubly linked list with O(1) push/pop at both ends.
 *
 * Every node keeps a `prev` and `next` pointer, which is what buys
 * O(1) pop_back (unlike SinglyLinkedList, where finding the new tail
 * requires an O(n) walk). The extra pointer per node is the classic
 * space/time trade-off students should be able to name.
 *
 * @tparam T Element type stored by value in each node.
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

    // Shared implementation for the mutable and const bidirectional iterators.
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

    /** @brief Reverse iterator built on top of the bidirectional Iterator. */
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
        for (const auto& value : values) {
            push_back(value);
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
        Node* node = new Node(value, nullptr, head_);
        if (head_ != nullptr) {
            head_->prev = node;
        } else {
            tail_ = node;
        }
        head_ = node;
        ++size_;
    }

    void push_back(const T& value) {
        Node* node = new Node(value, tail_, nullptr);
        if (tail_ != nullptr) {
            tail_->next = node;
        } else {
            head_ = node;
        }
        tail_ = node;
        ++size_;
    }

    void pop_front() {
        if (head_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::pop_front: list is empty");
        }
        Node* old_head = head_;
        head_ = head_->next;
        if (head_ != nullptr) {
            head_->prev = nullptr;
        } else {
            tail_ = nullptr;
        }
        delete old_head;
        --size_;
    }

    void pop_back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::pop_back: list is empty");
        }
        Node* old_tail = tail_;
        tail_ = tail_->prev;
        if (tail_ != nullptr) {
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }
        delete old_tail;
        --size_;
    }

    /** @brief Inserts `value` so it becomes the element at `index`. */
    void insert_at(std::size_t index, const T& value) {
        if (index > size_) {
            throw std::out_of_range("DoublyLinkedList::insert_at: index out of range");
        }
        if (index == 0) {
            push_front(value);
            return;
        }
        if (index == size_) {
            push_back(value);
            return;
        }
        Node* at = node_at(index);
        Node* prev = at->prev;
        Node* node = new Node(value, prev, at);
        prev->next = node;
        at->prev = node;
        ++size_;
    }

    /** @brief Removes the element located at `index`. */
    void erase_at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("DoublyLinkedList::erase_at: index out of range");
        }
        Node* target = node_at(index);
        if (target->prev != nullptr) {
            target->prev->next = target->next;
        } else {
            head_ = target->next;
        }
        if (target->next != nullptr) {
            target->next->prev = target->prev;
        } else {
            tail_ = target->prev;
        }
        delete target;
        --size_;
    }

    // --- Accessors ---

    T& front() {
        if (head_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::front: list is empty");
        }
        return head_->data;
    }

    const T& front() const {
        if (head_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::front: list is empty");
        }
        return head_->data;
    }

    T& back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::back: list is empty");
        }
        return tail_->data;
    }

    const T& back() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("DoublyLinkedList::back: list is empty");
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
        // Walk from whichever end is closer to cut the traversal in half.
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
        for (const auto& value : other) {
            push_back(value);
        }
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace dsa

#endif  // DSA_LINEAR_DOUBLY_LINKED_LIST_HPP
