#ifndef BOOKSTORAGE_HPP
#define BOOKSTORAGE_HPP

#include "BookIndexManager.hpp"
#include "FileOperation.hpp"
#include <cstring>
#include <sstream>
#include <vector>
#include <string>

class Book {
private:
    struct BookRecord {
        int id;
        char ISBN[21];
        char BookName[61];
        char Author[61];
        char Keyword[61];
        int quantity;
        float price;
    };

    FileOperation<BookRecord> bookFile;
    
    IndexManager<std::string> ISBNIndex;
    IndexManager<std::string> nameIndex;
    IndexManager<std::string> authorIndex;
    IndexManager<std::string> keywordIndex;
    IndexManager<float> priceIndex;
    
    int nextId;

public:
    Book() : 
        bookFile("books.dat"),
        ISBNIndex("isbn_index.dat"),
        nameIndex("name_index.dat"),
        authorIndex("author_index.dat"),
        keywordIndex("keyword_index.dat"),
        priceIndex("price_index.dat"),
        nextId(0) {
        
        if (bookFile.isEmpty()) {
            BookRecord dummy;
            dummy.id = -1;
            bookFile.write(dummy);
            nextId = 0;
        } else {
            BookRecord lastRecord;
            bookFile.read(lastRecord, bookFile.getSize() - sizeof(BookRecord));
            nextId = lastRecord.id + 1;
        }
    }

    void addBook(const std::string& ISBN, const std::string& name, 
                const std::string& author, const std::string& keywords,
                int qty, float price) {
        BookRecord record;
        record.id = nextId++;
        strncpy(record.ISBN, ISBN.c_str(), 20);
        strncpy(record.BookName, name.c_str(), 60);
        strncpy(record.Author, author.c_str(), 60);
        strncpy(record.Keyword, keywords.c_str(), 60);
        record.quantity = qty;
        record.price = price;

        int pos = bookFile.write(record);

        ISBNIndex.insert(ISBN, record.id);
        nameIndex.insert(name, record.id);
        authorIndex.insert(author, record.id);
        
        std::string keyword;
        std::istringstream keywordStream(keywords);
        while (std::getline(keywordStream, keyword, '|')) {
            keywordIndex.insert(keyword, record.id);
        }
        
        priceIndex.insert(price, record.id);
    }

    std::vector<BookRecord> findByISBN(const std::string& ISBN) {
        std::vector<BookRecord> results;
        auto ids = ISBNIndex.find(ISBN);
        for (int id : ids) {
            BookRecord record;
            bookFile.read(record, id);
            results.push_back(record);
        }
        return results;
    }

    std::vector<BookRecord> findByAuthor(const std::string& author) {
        std::vector<BookRecord> results;
        auto ids = authorIndex.find(author);
        for (int id : ids) {
            BookRecord record;
            bookFile.read(record, id);
            results.push_back(record);
        }
        return results;
    }

    std::vector<BookRecord> findByKeyword(const std::string& keyword) {
        std::vector<BookRecord> results;
        auto ids = keywordIndex.find(keyword);
        for (int id : ids) {
            BookRecord record;
            bookFile.read(record, id);
            results.push_back(record);
        }
        return results;
    }

    std::vector<BookRecord> findByName(const std::string& name) {
        std::vector<BookRecord> results;
        auto ids = nameIndex.find(name);
        for (int id : ids) {
            BookRecord record;
            bookFile.read(record, id);
            results.push_back(record);
        }
        return results;
    }

    void deleteBook(const std::string& ISBN) {
        auto ids = ISBNIndex.find(ISBN);
        if (ids.empty()) return;
        
        int id = *ids.begin();
        BookRecord record;
        bookFile.read(record, id);

        ISBNIndex.remove(record.ISBN, id);
        nameIndex.remove(record.BookName, id);
        authorIndex.remove(record.Author, id);
        
        std::string keyword;
        std::istringstream keywordStream(record.Keyword);
        while (std::getline(keywordStream, keyword, '|')) {
            keywordIndex.remove(keyword, id);
        }
        
        priceIndex.remove(record.price, id);
    }

    // show, buy, select, modify, import
};

#endif // BOOKSTORAGE_HPP