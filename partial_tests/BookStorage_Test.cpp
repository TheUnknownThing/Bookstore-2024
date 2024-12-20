#include "../src/include/BookStorage.hpp"
#include <iostream>

int main() {
    Book bookManager;

    std::cout << "Add books" << std::endl;

    // Add books
    bookManager.addBook("978-1-4028-9462-6", "C++ Primer", "Lippman", "Programming|C++", 10, 9.99);
    bookManager.addBook("978-0-321-56384-2", "Effective Modern C++", "Scott Meyers", "Programming|C++", 5, 12.99);

    // Show all books
    std::cout << "All books:" << std::endl;
    bookManager.show();

    // Find by ISBN
    auto results = bookManager.findByISBN("978-1-4028-9462-6");
    std::cout << "\nFind by ISBN (978-1-4028-9462-6): " << results.size() << " result(s)" << std::endl;

    // Modify a book's name
    bookManager.modify("978-1-4028-9462-6", "", "C++ Primer (5th Edition)");
    std::cout << "\nAfter modification:" << std::endl;
    bookManager.show();

    // Attempt a buy
    bool buySuccess = bookManager.buy("978-1-4028-9462-6", 2);
    std::cout << "\nBuy success: " << (buySuccess ? "Yes" : "No") << std::endl;
    bookManager.show();

    // Delete a book
    bookManager.deleteBook("978-0-321-56384-2");
    std::cout << "\nAfter deletion:" << std::endl;
    bookManager.show();

    return 0;
}