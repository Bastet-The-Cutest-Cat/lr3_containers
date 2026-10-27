#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Класс двусвязного списка. T — тип хранимых элементов.
template <typename T>
class DoublyLinkedList {
private:
    // Внутренняя структура узла списка.
    // Каждый узел хранит данные и указатели на соседние узлы.
    struct Node {
        T data;           // сами данные
        Node* previous;   // указатель на предыдущий узел (nullptr для первого)
        Node* next;       // указатель на следующий узел (nullptr для последнего)

        // Конструктор узла с копированием значения.
        explicit Node(const T& value) : data(value), previous(nullptr), next(nullptr) {}
        // Конструктор узла с перемещением значения (чтобы не копировать лишний раз).
        explicit Node(T&& value) : data(std::move(value)), previous(nullptr), next(nullptr) {}
    };

public:
    // Итератор для прохода по списку.
    // Реализован как вложенный класс, хранит указатель на текущий узел.
    class iterator {
        friend class DoublyLinkedList; // чтобы список мог создавать итератор из Node*
        Node* node_ = nullptr;         // текущий узел (nullptr = конец списка)

        // Приватный конструктор — снаружи итератор можно создать только через begin()/end().
        explicit iterator(Node* node) : node_(node) {}

    public:
        // Конструктор по умолчанию — «пустой» итератор.
        iterator() = default;

        // Разыменование итератора: возвращает ссылку на данные.
        // Если итератор невалидный (nullptr), бросаем исключение.
        T& operator*() const {
            if (node_ == nullptr) throw std::out_of_range("Invalid iterator");
            return node_->data;
        }

        // Префиксный инкремент: переходим к следующему узлу.
        iterator& operator++() { if (node_) node_ = node_->next; return *this; }

        // Сравнение итераторов по указателю на узел.
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return node_ != other.node_; }
    };

    // Конструктор по умолчанию — пустой список (head_ и tail_ = nullptr, size_ = 0).
    explicit DoublyLinkedList() = default;

    // Конструктор перемещения: «забираем» данные из другого списка.
    // Другой список при этом становится пустым.
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    // Деструктор: освобождаем всю память, выделенную под узлы.
    ~DoublyLinkedList() { clear(); }

    // Оператор присваивания с перемещением.
    // Сначала очищаем себя, потом забираем данные у other.
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) { // защита от самоприсваивания
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

    // Копирование запрещено (чтобы не было двойного освобождения памяти).
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // Возвращает количество элементов в списке.
    std::size_t size() const { return size_; }
    // Проверка на пустоту.
    bool empty() const { return size_ == 0; }

    // Добавление в конец: вставка по индексу size_ (то есть после последнего).
    void push_back(const T& value) { insert(size_, value); }
    void push_back(T&& value) { insert(size_, std::move(value)); }

    // Добавление в начало: вставка по индексу 0.
    void push_front(const T& value) { insert(0, value); }
    void push_front(T&& value) { insert(0, std::move(value)); }

    // Вставка элемента по индексу (с копированием или перемещением значения).
    void insert(std::size_t index, const T& value) { insert_node(index, new Node(value)); }
    void insert(std::size_t index, T&& value) { insert_node(index, new Node(std::move(value))); }

    // Удаление элемента по индексу.
    void erase(std::size_t index) {
        check_index(index);
        Node* node = get_node(index);

        // Перевязываем соседей: предыдущий узел теперь указывает на следующий и наоборот.
        if (node->previous) node->previous->next = node->next;
        else head_ = node->next; // если удаляем первый — сдвигаем head_

        if (node->next) node->next->previous = node->previous;
        else tail_ = node->previous; // если удаляем последний — сдвигаем tail_

        delete node;   // освобождаем память удалённого узла
        --size_;       // уменьшаем размер
    }

    // Доступ к элементу по индексу (неконстантная версия).
    T& operator[](std::size_t index) { check_index(index); return get_node(index)->data; }
    // Константная версия — для работы с const-объектами.
    const T& operator[](std::size_t index) const { check_index(index); return get_node(index)->data; }

    // Итератор на первый элемент (или end(), если список пуст).
    iterator begin() { return iterator(head_); }
    // Итератор «после последнего» — nullptr.
    iterator end() { return iterator(nullptr); }

private:
    Node* head_ = nullptr;      // первый узел списка
    Node* tail_ = nullptr;      // последний узел списка
    std::size_t size_ = 0;      // количество элементов

    // Проверка, что индекс не выходит за границы (для доступа и удаления).
    void check_index(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("Index is out of range");
    }

    // Получение указателя на узел по индексу.
    // Проходим от головы, пока не дойдём до нужного узла.
    Node* get_node(std::size_t index) const {
        Node* node = head_;
        for (std::size_t i = 0; i < index; ++i) node = node->next;
        return node;
    }

    // Вставка готового узла по индексу.
    // Выделенный Node* уже создан снаружи — здесь только «вставляем» его в нужное место.
    void insert_node(std::size_t index, Node* node) {
        // Для вставки допустим индекс == size_ (вставка в конец),
        // но больше — уже ошибка. Если ошиблись, не забываем удалить узел.
        if (index > size_) {
            delete node;
            throw std::out_of_range("Insert index is out of range");
        }

        if (index == 0) {
            // Вставка в начало.
            node->next = head_;
            if (head_) head_->previous = node;
            else tail_ = node; // если список был пуст — новый узел становится и tail_
            head_ = node;
        } else if (index == size_) {
            // Вставка в конец.
            node->previous = tail_;
            if (tail_) tail_->next = node;
            else head_ = node; // если список был пуст — новый узел становится и head_
            tail_ = node;
        } else {
            // Вставка в середину: находим текущий узел на позиции index,
            // вставляем новый перед ним.
            Node* current = get_node(index);
            node->previous = current->previous;
            node->next = current;
            current->previous->next = node; // предыдущий сосед теперь указывает на новый
            current->previous = node;       // текущий указывает назад на новый
        }
        ++size_;
    }

    // Очистка списка: проходим по всем узлам и удаляем их.
    // head_ всё время сдвигается к следующему узлу, пока не станет nullptr.
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