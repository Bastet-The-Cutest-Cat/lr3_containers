#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

// Класс односвязного списка. T — тип хранимых элементов.
// В отличие от двусвязного, каждый узел хранит только указатель на следующий.
template <typename T>
class SinglyLinkedList {
private:
    // Внутренняя структура узла: данные + указатель на следующий узел.
    struct Node {
        T data;        // сами данные
        Node* next;    // указатель на следующий узел (nullptr для последнего)

        // Конструктор с копированием значения.
        explicit Node(const T& value) : data(value), next(nullptr) {}
        // Конструктор с перемещением значения (чтобы не копировать лишний раз).
        explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };

public:
    // Итератор для прохода по списку: хранит указатель на текущий узел.
    class iterator {
        friend class SinglyLinkedList; // чтобы список мог создавать итератор из Node*
        Node* node_ = nullptr;         // текущий узел (nullptr = конец)

        // Приватный конструктор — снаружи итератор создаётся только через begin()/end().
        explicit iterator(Node* node) : node_(node) {}

    public:
        // Конструктор по умолчанию — «пустой» итератор.
        iterator() = default;

        // Разыменование: возвращает ссылку на данные.
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
    explicit SinglyLinkedList() = default;

    // Конструктор перемещения: «забираем» данные из другого списка.
    // Другой список при этом становится пустым.
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    // Деструктор: освобождаем всю память, выделенную под узлы.
    ~SinglyLinkedList() { clear(); }

    // Оператор присваивания с перемещением.
    // Сначала очищаем себя, потом забираем данные у other.
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
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
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

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

    // Вставка элемента по индексу (копирование или перемещение значения).
    void insert(std::size_t index, const T& value) { insert_node(index, new Node(value)); }
    void insert(std::size_t index, T&& value) { insert_node(index, new Node(std::move(value))); }

    // Удаление элемента по индексу.
    void erase(std::size_t index) {
        check_index(index);
        Node* previous = nullptr;   // узел перед удаляемым
        Node* current = head_;      // удаляемый узел

        // Доходим до удаляемого узла, запоминая предыдущий.
        for (std::size_t i = 0; i < index; ++i) {
            previous = current;
            current = current->next;
        }

        // Перевязываем: предыдущий теперь указывает через удаляемый.
        if (previous) previous->next = current->next;
        else head_ = current->next; // если удаляем первый — сдвигаем head_

        // Если удаляли последний элемент, обновляем tail_.
        if (tail_ == current) tail_ = previous;

        delete current;   // освобождаем память
        --size_;          // уменьшаем размер
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

    // Проверка, что индекс не выходит за границы.
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
    // Node* уже создан снаружи — здесь только «вставляем» его в нужное место.
    void insert_node(std::size_t index, Node* node) {
        // Для вставки допустим index == size_ (вставка в конец), больше — ошибка.
        // При ошибке не забываем удалить узел, чтобы не было утечки.
        if (index > size_) {
            delete node;
            throw std::out_of_range("Insert index is out of range");
        }

        if (index == 0) {
            // Вставка в начало.
            node->next = head_;
            head_ = node;
            if (size_ == 0) tail_ = node; // если список был пуст — новый узел и tail_
        } else if (index == size_) {
            // Вставка в конец: tail_ уже существует, т.к. index > 0.
            tail_->next = node;
            tail_ = node;
        } else {
            // Вставка в середину: находим узел перед позицией index
            // и вставляем новый сразу после него.
            Node* previous = get_node(index - 1);
            node->next = previous->next;
            previous->next = node;
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