#ifndef DSA_LINEAR_CIRCULAR_LINKED_LIST_HPP
#define DSA_LINEAR_CIRCULAR_LINKED_LIST_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "dsa/interfaces/collection.hpp"

namespace dsa {

/**
 * @brief Singly linked, circularly linked list: the last node points back
 * to the first instead of to nullptr.
 *
 * Only a `tail_` pointer is kept (tail_->next is the head), which is the
 * usual textbook trick: it gives O(1) access to both ends without a
 * separate head pointer. Because there is no natural "past-the-end"
 * node, iteration cannot rely on comparing against nullptr like the
 * other lists in this module; Iterator instead tracks how many steps it
 * has taken and stops after `size()` elements, which is what keeps
 * range-for from looping forever.
 *
 * @tparam T Element type stored by value in each node.
 */
template <typename T>
class CircularLinkedList : public Collection<T> {
   private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value, Node* next_ = nullptr) : data(value), next(next_) {}
    };

   public:
    /**
     * @brief Forward iterator that stops after visiting `size()` nodes.
     *
     * A default-constructed Iterator (steps_remaining_ == 0) represents
     * end(); this is what lets a range spanning the whole list terminate
     * even though the underlying node chain never hits nullptr.
     */
    class Iterator {
       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() : node_(nullptr), steps_remaining_(0) {}
        Iterator(Node* node, std::size_t steps_remaining)
            : node_(node), steps_remaining_(steps_remaining) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }

        Iterator& operator++() {
            node_ = node_->next;
            --steps_remaining_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return steps_remaining_ == other.steps_remaining_ &&
                   (steps_remaining_ == 0 || node_ == other.node_);
        }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

       private:
        Node* node_;
        std::size_t steps_remaining_;
    };

    /** @brief Read-only counterpart of Iterator; same stop-after-`size()` logic. */
    class ConstIterator {
       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator() : node_(nullptr), steps_remaining_(0) {}
        ConstIterator(const Node* node, std::size_t steps_remaining)
            : node_(node), steps_remaining_(steps_remaining) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }

        ConstIterator& operator++() {
            node_ = node_->next;
            --steps_remaining_;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const {
            return steps_remaining_ == other.steps_remaining_ &&
                   (steps_remaining_ == 0 || node_ == other.node_);
        }
        bool operator!=(const ConstIterator& other) const { return !(*this == other); }

       private:
        const Node* node_;
        std::size_t steps_remaining_;
    };

    CircularLinkedList() = default;

    CircularLinkedList(std::initializer_list<T> values) {
        for (const auto& value : values) {
            push_back(value);
        }
    }

    CircularLinkedList(const CircularLinkedList& other) { copy_from(other); }

    CircularLinkedList& operator=(const CircularLinkedList& other) {
        if (this != &other) {
            CircularLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    CircularLinkedList(CircularLinkedList&& other) noexcept
        : tail_(other.tail_), size_(other.size_) {
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    CircularLinkedList& operator=(CircularLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            tail_ = other.tail_;
            size_ = other.size_;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~CircularLinkedList() override { clear(); }

    // --- Collection interface ---

    std::size_t size() const noexcept override { return size_; }

    void clear() override {
        if (tail_ == nullptr) {
            return;
        }
        Node* current = tail_->next;  // head
        for (std::size_t i = 0; i < size_; ++i) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    // --- Modifiers ---

    void push_front(const T& value) {
        if (tail_ == nullptr) {
            Node* node = new Node(value);
            node->next = node;
            tail_ = node;
        } else {
            Node* node = new Node(value, tail_->next);
            tail_->next = node;
        }
        ++size_;
    }

    void push_back(const T& value) {
        push_front(value);
        tail_ = tail_->next;
    }

    void pop_front() {
        if (tail_ == nullptr) {
            throw std::out_of_range("CircularLinkedList::pop_front: list is empty");
        }
        Node* head = tail_->next;
        if (head == tail_) {
            delete head;
            tail_ = nullptr;
        } else {
            tail_->next = head->next;
            delete head;
        }
        --size_;
    }

    /** @brief Rotates the list by `n` positions (positive: head advances forward). */
    void rotate(long long n) {
        if (size_ == 0) {
            return;
        }
        long long steps = n % static_cast<long long>(size_);
        if (steps < 0) {
            steps += static_cast<long long>(size_);
        }
        for (long long i = 0; i < steps; ++i) {
            tail_ = tail_->next;
        }
    }

    // --- Accessors ---

    T& front() {
        if (tail_ == nullptr) {
            throw std::out_of_range("CircularLinkedList::front: list is empty");
        }
        return tail_->next->data;
    }

    const T& front() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("CircularLinkedList::front: list is empty");
        }
        return tail_->next->data;
    }

    T& back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("CircularLinkedList::back: list is empty");
        }
        return tail_->data;
    }

    const T& back() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("CircularLinkedList::back: list is empty");
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

    // --- Iterators ---

    Iterator begin() {
        if (tail_ == nullptr) {
            return end();
        }
        return Iterator(tail_->next, size_);
    }
    Iterator end() { return Iterator(); }

    ConstIterator begin() const {
        if (tail_ == nullptr) {
            return end();
        }
        return ConstIterator(tail_->next, size_);
    }
    ConstIterator end() const { return ConstIterator(); }
    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend() const { return end(); }

    void swap(CircularLinkedList& other) noexcept {
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

   private:
    void copy_from(const CircularLinkedList& other) {
        for (const auto& value : other) {
            push_back(value);
        }
    }

    Node* tail_ = nullptr;  // tail_->next is head, when non-null.
    std::size_t size_ = 0;
};

}  // namespace dsa

#endif  // DSA_LINEAR_CIRCULAR_LINKED_LIST_HPP
