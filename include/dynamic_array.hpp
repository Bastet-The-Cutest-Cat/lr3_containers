#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Класс динамического массива (аналог std::vector, упрощённый).
// T — тип хранимых элементов.
template <typename T>
class DynamicArray {
public:
    // Итератор для неконстантного массива — просто указатель на T.
    using iterator = T*;
    // Константный итератор — указатель на const T.
    using const_iterator = const T*;

    // Конструктор по умолчанию: пустой массив, память ещё не выделена.
    explicit DynamicArray() = default;

    // Конструктор перемещения: «забираем» буфер у другого объекта.
    // Другой массив становится пустым (не владеет памятью).
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Деструктор: освобождаем выделенный буфер (если он был).
    ~DynamicArray() { delete[] data_; }

    // Оператор присваивания с перемещением.
    // Сначала освобождаем свой буфер, затем забираем чужой.
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) { // защита от самоприсваивания
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

    // Копирование запрещено (чтобы избежать двойного освобождения памяти).
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    // Возвращает количество элементов.
    std::size_t size() const { return size_; }
    // Проверка на пустоту.
    bool empty() const { return size_ == 0; }

    // Добавление в конец: вставка по индексу size_ (после последнего элемента).
    void push_back(const T& value) { insert(size_, value); }
    void push_back(T&& value) { insert(size_, std::move(value)); }
    // Добавление в начало: вставка по индексу 0.
    void push_front(const T& value) { insert(0, value); }
    void push_front(T&& value) { insert(0, std::move(value)); }

    // Вставка элемента с копированием значения.
    void insert(std::size_t index, const T& value) {
        insert_value(index, value);
    }

    // Вставка элемента с перемещением значения.
    void insert(std::size_t index, T&& value) {
        insert_value(index, std::move(value));
    }

    // Удаление элемента по индексу.
    // Все элементы справа сдвигаются на одну позицию влево.
    void erase(std::size_t index) {
        check_index(index);
        for (std::size_t i = index; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        --size_;
    }

    // Доступ к элементу по индексу (неконстантная версия).
    T& operator[](std::size_t index) {
        check_index(index);
        return data_[index];
    }

    // Константная версия operator[] — для работы с const-объектами.
    const T& operator[](std::size_t index) const {
        check_index(index);
        return data_[index];
    }

    // Итератор на первый элемент.
    iterator begin() { return data_; }
    // Итератор «после последнего». Если буфера нет — возвращаем nullptr.
    iterator end() { return data_ == nullptr ? nullptr : data_ + size_; }
    // Константные версии для const-объектов.
    const_iterator begin() const { return data_; }
    const_iterator end() const { return data_ == nullptr ? nullptr : data_ + size_; }

private:
    T* data_ = nullptr;          // указатель на выделенный буфер
    std::size_t size_ = 0;       // сколько элементов сейчас в массиве
    std::size_t capacity_ = 0;   // сколько элементов помещается в буфер без перевыделения

    // Проверка, что индекс не выходит за границы (для доступа и удаления).
    void check_index(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("Index is out of range");
    }

    // Вставка значения по индексу.
    // Принимаем T по значению — потом его перемещаем внутрь (так удобнее,
    // снаружи уже сделали либо копию, либо move).
    void insert_value(std::size_t index, T value) {
        // Для вставки допустим index == size_ (вставка в конец), больше — ошибка.
        if (index > size_) throw std::out_of_range("Insert index is out of range");

        // Если буфер заполнен — расширяем его.
        // Новая ёмкость растёт примерно в 1.5 раза: cap + cap/2 + 1.
        if (size_ == capacity_) {
            std::size_t new_capacity = capacity_ == 0 ? 4 : capacity_ + capacity_ / 2 + 1;
            T* new_data = new T[new_capacity];

            // Переносим существующие элементы в новый буфер (перемещением — быстрее).
            for (std::size_t i = 0; i < size_; ++i)
                new_data[i] = std::move(data_[i]);

            // Освобождаем старый буфер и переключаемся на новый.
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }

        // Сдвигаем элементы [index, size_) на одну позицию вправо,
        // чтобы освободить место под index.
        for (std::size_t i = size_; i > index; --i)
            data_[i] = std::move(data_[i - 1]);

        // Записываем новое значение на освободившееся место.
        data_[index] = std::move(value);
        ++size_;
    }
};