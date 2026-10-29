#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

public:
    class iterator {
        friend class SinglyLinkedList;
        Node* node_ = nullptr;
        explicit iterator(Node* node) : node_(node) {}
    public:
        iterator() = default;
        T& operator*() const {
            if (node_ == nullptr) throw std::out_of_range("Invalid iterator");
            return node_->data;
        }
        iterator& operator++() { if (node_) node_ = node_->next; return *this; }
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return node_ != other.node_; }
    };

    explicit SinglyLinkedList() = default;
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    ~SinglyLinkedList() { clear(); }

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
    // Конструктор копирования.
    SinglyLinkedList(const SinglyLinkedList& other) {
        try {
            for (Node* node = other.head_; node != nullptr; node = node->next)
                push_back(node->data);
        } catch (...) {
            clear();
            throw;
        }
    }
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void push_back(const T& value) { insert(size_, value); }
    void push_back(T&& value) { insert(size_, std::move(value)); }
    void push_front(const T& value) { insert(0, value); }
    void push_front(T&& value) { insert(0, std::move(value)); }
    void insert(std::size_t index, const T& value) { insert_node(index, new Node(value)); }
    void insert(std::size_t index, T&& value) { insert_node(index, new Node(std::move(value))); }

    void erase(std::size_t index) {
        check_index(index);
        Node* previous = nullptr;
        Node* current = head_;
        for (std::size_t i = 0; i < index; ++i) {
            previous = current;
            current = current->next;
        }
        if (previous) previous->next = current->next;
        else head_ = current->next;
        if (tail_ == current) tail_ = previous;
        delete current;
        --size_;
    }

    T& operator[](std::size_t index) { check_index(index); return get_node(index)->data; }
    const T& operator[](std::size_t index) const { check_index(index); return get_node(index)->data; }
    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;

    void check_index(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("Index is out of range");
    }
    Node* get_node(std::size_t index) const {
        Node* node = head_;
        for (std::size_t i = 0; i < index; ++i) node = node->next;
        return node;
    }
    void insert_node(std::size_t index, Node* node) {
        if (index > size_) {
            delete node;
            throw std::out_of_range("Insert index is out of range");
        }
        if (index == 0) {
            node->next = head_;
            head_ = node;
            if (size_ == 0) tail_ = node;
        } else if (index == size_) {
            tail_->next = node;
            tail_ = node;
        } else {
            Node* previous = get_node(index - 1);
            node->next = previous->next;
            previous->next = node;
        }
        ++size_;
    }
    void clear() {
        while (head_) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }
};
