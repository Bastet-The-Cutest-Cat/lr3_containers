#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "dynamic_array.hpp"
#include "doubly_linked_list.hpp"
#include "singly_linked_list.hpp"

void check(bool condition) {
    if (!condition) {
        std::cerr << "Test failed\n";
        std::exit(1);
    }
}

template <typename Container>
void test_container() {
    Container values;
    check(values.empty());

    for (int i = 0; i < 10; ++i) values.push_back(i);
    check(values.size() == 10);
    for (std::size_t i = 0; i < 10; ++i) check(values[i] == static_cast<int>(i));

    values.erase(6);
    values.erase(4);
    values.erase(2);
    const int after_erase[] = {0, 1, 3, 5, 7, 8, 9};
    check(values.size() == 7);
    for (std::size_t i = 0; i < 7; ++i) check(values[i] == after_erase[i]);

    values.push_front(10);
    values.insert(values.size() / 2, 20);
    values.push_back(30);
    const int expected[] = {10, 0, 1, 3, 20, 5, 7, 8, 9, 30};
    check(values.size() == 10);
    for (std::size_t i = 0; i < 10; ++i) check(values[i] == expected[i]);

    std::size_t count = 0;
    for (auto it = values.begin(); it != values.end(); ++it) {
        check(*it == expected[count]);
        ++count;
    }
    check(count == 10);
}

int main() {
    test_container<DynamicArray<int>>();
    test_container<DoublyLinkedList<int>>();
    test_container<SinglyLinkedList<int>>();

    std::string text = "test";
    DynamicArray<std::string> source;
    source.push_back(std::move(text));
    DynamicArray<std::string> destination(std::move(source));
    check(destination.size() == 1);
    check(destination[0] == "test");
    check(source.empty());

    std::cout << "All tests passed\n";
    return 0;
}
