#include <iostream>
#include <string>
#include "BlockList.hpp"

int main() {

    // freopen("test/2.in", "r", stdin);

    // freopen("book.out", "w", stdout);

    int n;
    std::cin >> n;
    std::string op;

    Book book;

    for (int i = 0; i < n; ++i) {
        std::cin >> op;
        // insert, delete, find
        if (op == "insert") {
            std::string index;
            int value;
            std::cin >> index >> value;
            book.Insert(index, value);
        } else if (op == "delete") {
            std::string index;
            int value;
            std::cin >> index >> value;
            book.Delete(index, value);
        } else if (op == "find") {
            std::string index;
            std::cin >> index;
            //std::cout << "find " << index << std::endl; // debug
            book.Find(index);
        } else if (op == "print") {
            book.PrintAll(); // debug
        }
    }
    return 0;
}