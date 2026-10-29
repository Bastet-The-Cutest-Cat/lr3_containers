#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T>
class DynamicArray {
public:
    class iterator {
        friend class DynamicArray;
        T* pointer_ = nullptr;
        explicit iterator(T* pointer) : pointer_(pointer) {}

    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        iterator() = default;
        reference operator*() const { return *pointer_; }
        pointer operator->() const { return pointer_; }
        iterator& operator++() { ++pointer_; return *this; }
        iterator operator++(int) { iterator copy = *this; ++pointer_; return copy; }
        bool operator==(const iterator& other) const { return pointer_ == other.pointer_; }
        bool operator!=(const iterator& other) const { return pointer_ != other.pointer_; }
    };

    class const_iterator {
        friend class DynamicArray;
        const T* pointer_ = nullptr;
        explicit const_iterator(const T* pointer) : pointer_(pointer) {}

    public:
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() = default;
        reference operator*() const { return *pointer_; }
        pointer operator->() const { return pointer_; }
        const_iterator& operator++() { ++pointer_; return *this; }
        const_iterator operator++(int) { const_iterator copy = *this; ++pointer_; return copy; }
        bool operator==(const const_iterator& other) const { return pointer_ == other.pointer_; }
        bool operator!=(const const_iterator& other) const { return pointer_ != other.pointer_; }
    };

    DynamicArray() = default;

    // Конструктор копирования.
    DynamicArray(const DynamicArray& other)
        : data_(other.capacity_ ? new T[other.capacity_] : nullptr),
          size_(other.size_), capacity_(other.capacity_) {
        try {
            for (std::size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
        } catch (...) {
            delete[] data_;
            data_ = nullptr;
            throw;
        }
    }

    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~DynamicArray() { delete[] data_; }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    DynamicArray& operator=(const DynamicArray&) = delete;

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void push_back(const T& value) { insert(size_, value); }
    void push_back(T&& value) { insert(size_, std::move(value)); }
    void push_front(const T& value) { insert(0, value); }
    void push_front(T&& value) { insert(0, std::move(value)); }

    void insert(std::size_t index, const T& value) { insert_value(index, value); }
    void insert(std::size_t index, T&& value) { insert_value(index, std::move(value)); }

    void erase(std::size_t index) {
        check_index(index);
        for (std::size_t i = index; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        --size_;
    }

    T& operator[](std::size_t index) { check_index(index); return data_[index]; }
    const T& operator[](std::size_t index) const { check_index(index); return data_[index]; }

    // Итераторы на начало и конец массива.
    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ == nullptr ? nullptr : data_ + size_); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ == nullptr ? nullptr : data_ + size_); }

private:
    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

    void check_index(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("Index is out of range");
    }

    void insert_value(std::size_t index, T value) {
        if (index > size_) throw std::out_of_range("Insert index is out of range");

        if (size_ == capacity_) {
            std::size_t new_capacity = capacity_ == 0 ? 4 : capacity_ + capacity_ / 2 + 1;
            T* new_data = new T[new_capacity];
            for (std::size_t i = 0; i < size_; ++i) new_data[i] = std::move(data_[i]);
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }

        for (std::size_t i = size_; i > index; --i) data_[i] = std::move(data_[i - 1]);
        data_[index] = std::move(value);
        ++size_;
    }
};
