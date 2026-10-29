#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* previous;
        Node* next;
        explicit Node(const T& value) : data(value), previous(nullptr), next(nullptr) {}
        explicit Node(T&& value) : data(std::move(value)), previous(nullptr), next(nullptr) {}
    };

public:
    class iterator {
        friend class DoublyLinkedList;
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

    explicit DoublyLinkedList() = default;
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    ~DoublyLinkedList() { clear(); }

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
    // Конструктор копирования.
    DoublyLinkedList(const DoublyLinkedList& other) {
        try {
            for (Node* node = other.head_; node != nullptr; node = node->next)
                push_back(node->data);
        } catch (...) {
            clear();
            throw;
        }
    }
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

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
        Node* node = get_node(index);
        if (node->previous) node->previous->next = node->next;
        else head_ = node->next;
        if (node->next) node->next->previous = node->previous;
        else tail_ = node->previous;
        delete node;
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
            if (head_) head_->previous = node;
            else tail_ = node;
            head_ = node;
        } else if (index == size_) {
            node->previous = tail_;
            if (tail_) tail_->next = node;
            else head_ = node;
            tail_ = node;
        } else {
            Node* current = get_node(index);
            node->previous = current->previous;
            node->next = current;
            current->previous->next = node;
            current->previous = node;
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
