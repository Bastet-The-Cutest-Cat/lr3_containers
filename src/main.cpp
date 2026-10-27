#include <iostream>
#include <string>
#include <utility>

#include "dynamic_array.hpp"
#include "doubly_linked_list.hpp"
#include "singly_linked_list.hpp"

template <typename Container>
void print_container(const std::string& title, Container& values) {
    std::cout << title << ": ";
    bool first = true;
    for (auto it = values.begin(); it != values.end(); ++it) {
        if (!first) std::cout << ", ";
        std::cout << *it;
        first = false;
    }
    std::cout << '\n';
}

template <typename Container>
void demonstrate(const std::string& name) {
    Container values;
    for (int i = 0; i < 10; ++i) values.push_back(i);

    std::cout << "\n=== " << name << " ===\n";
    print_container("Initial", values);
    std::cout << "Size: " << values.size() << '\n';

    // Удаляем исходные элементы 7, 5 и 3 справа налево, чтобы индексы не смещались.
    values.erase(6);
    values.erase(4);
    values.erase(2);
    print_container("After erase", values);

    values.push_front(10);
    print_container("After push_front(10)", values);

    values.insert(values.size() / 2, 20);
    print_container("After insert(20)", values);

    values.push_back(30);
    print_container("After push_back(30)", values);
    std::cout << "Final size: " << values.size() << '\n';
}

int main() {
    demonstrate<DynamicArray<int>>("Dynamic array");
    demonstrate<DoublyLinkedList<int>>("Doubly linked list");
    demonstrate<SinglyLinkedList<int>>("Singly linked list");

    std::string word = "move";
    DynamicArray<std::string> first;
    first.push_back(std::move(word));
    DynamicArray<std::string> second(std::move(first));
    std::cout << "\nMove constructor example: " << second[0] << '\n';
    return 0;
}
