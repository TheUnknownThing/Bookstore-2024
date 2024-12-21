#ifndef BOOKSTORAGE_HPP
#define BOOKSTORAGE_HPP

#include "BookIndexManager.hpp"
#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

  BookIndexManager ISBNIndex, nameIndex, authorIndex, keywordIndex;

  int nextId;

public:
  Book()
      : ISBNIndex("isbn_index.dat"), nameIndex("name_index.dat"),
        authorIndex("author_index.dat"), keywordIndex("keyword_index.dat"),
        nextId(0) {
    bookFile.initialise("book.dat");
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

  void addBook(const std::string &ISBN) {
    // TODO
    std::vector<int> ids = ISBNIndex.Find(ISBN);
    if (!ids.empty()) {
      std::cout << "Invalid" << std::endl;
      return;
    } else {
      BookRecord newRecord;
      newRecord.id = nextId++;
      strcpy(newRecord.ISBN, ISBN.c_str());
      bookFile.write(newRecord);
      ISBNIndex.Insert(ISBN, newRecord.id);
    }
  }

  std::vector<BookRecord> findByISBN(const std::string &ISBN) {
    std::vector<int> ids = ISBNIndex.Find(ISBN);
    std::vector<BookRecord> records;
    for (int id : ids) {
      BookRecord record;
      bookFile.read(record, id);
      records.push_back(record);
    }
    return records;
  }

  std::vector<BookRecord> getAll() {
    std::vector<int> ids = ISBNIndex.getAll();
    std::vector<BookRecord> records;
    for (int id : ids) {
      BookRecord record;
      bookFile.read(record, id);
      records.push_back(record);
    }
    return records;
  }

  std::vector<BookRecord> findByAuthor(const std::string &author) {
    std::vector<int> ids = authorIndex.Find(author);
    std::vector<BookRecord> records;
    for (int id : ids) {
      BookRecord record;
      bookFile.read(record, id);
      records.push_back(record);
    }
    return records;
  }

  std::vector<BookRecord> findByKeyword(const std::string &keyword) {
    std::vector<int> ids = keywordIndex.Find(keyword);
    std::vector<BookRecord> records;
    for (int id : ids) {
      BookRecord record;
      bookFile.read(record, id);
      records.push_back(record);
    }
    return records;
  }

  std::vector<BookRecord> findByName(const std::string &name) {
    std::vector<int> ids = nameIndex.Find(name);
    std::vector<BookRecord> records;
    for (int id : ids) {
      BookRecord record;
      bookFile.read(record, id);
      records.push_back(record);
    }
    return records;
  }

  void deleteBook(const std::string &ISBN) {
    std::vector<int> ids = ISBNIndex.Find(ISBN);
    if (ids.empty()) {
      return;
    } else {
      for (int id : ids) {
        BookRecord record;
        bookFile.read(record, id);
        ISBNIndex.Delete(record.ISBN, record.id);
        nameIndex.Delete(record.BookName, record.id);
        authorIndex.Delete(record.Author, record.id);
        keywordIndex.Delete(record.Keyword, record.id);
        bookFile.Delete(id * sizeof(BookRecord));
      }
    }
  }

  // show, buy, select, modify, import
  void show(const std::string &ISBN = "", const std::string &name = "",
            const std::string &author = "", const std::string &keyword = "") {
    std::vector<BookRecord> records;
    if (ISBN != "") {
      records = findByISBN(ISBN);
    } else if (name != "") {
      records = findByName(name);
    } else if (author != "") {
      records = findByAuthor(author);
    } else if (keyword != "") {
      records = findByKeyword(keyword);
    } else {
      // show all
      records = getAll();
    }
    for (const auto &record : records) {
      std::cout << record.ISBN << "\t" << record.BookName << "\t"
                << record.Author << "\t" << record.Keyword << "\t"
                << record.quantity << "\t" << record.price << std::endl;
    }
  }

  bool buy(const std::string &ISBN, int quantity) {
    std::vector<int> ids = ISBNIndex.Find(ISBN);
    if (ids.empty()) {
      return false;
    } else {
      BookRecord record;
      bookFile.read(record, ids[0]);
      if (record.quantity < quantity) {
        return false;
      }
      record.quantity -= quantity;
      bookFile.update(record, ids[0]);
      return true;
    }
  }

  bool modify(const std::string &currentISBN = "", const std::string &ISBN = "",
              const std::string &name = "", const std::string &author = "",
              const std::string &keyword = "", float price = 0) {
    std::vector<int> ids = ISBNIndex.Find(currentISBN);
    if (ids.empty()) {
      // std::cout << "Invalid" << std::endl;
      return false;
    } else {
      BookRecord record;
      bookFile.read(record, ids[0]);
      if (currentISBN == ISBN) {
        // std::cout << "Invalid" << std::endl;
        return false;
      }
      if (ISBN != "") {
        std::vector<int> newIds = ISBNIndex.Find(ISBN);
        if (!newIds.empty()) {
          // std::cout << "Invalid" << std::endl;
          return false;
        }
        ISBNIndex.Delete(currentISBN, record.id);
        ISBNIndex.Insert(ISBN, record.id);
        strcpy(record.ISBN, ISBN.c_str());
      } else if (name != "") {
        std::vector<int> newIds = nameIndex.Find(name);
        if (!newIds.empty()) {
          // std::cout << "Invalid" << std::endl;
          return false;
        }
        nameIndex.Delete(record.BookName, record.id);
        nameIndex.Insert(name, record.id);
        strcpy(record.BookName, name.c_str());
      } else if (author != "") {
        std::vector<int> newIds = authorIndex.Find(author);
        if (!newIds.empty()) {
          // std::cout << "Invalid" << std::endl;
          return false;
        }
        authorIndex.Delete(record.Author, record.id);
        authorIndex.Insert(author, record.id);
        strcpy(record.Author, author.c_str());
      } else if (keyword != "") {
        std::vector<int> newIds = keywordIndex.Find(keyword);
        if (!newIds.empty()) {
          // std::cout << "Invalid" << std::endl;
          return false;
        }
        keywordIndex.Delete(record.Keyword, record.id);
        keywordIndex.Insert(keyword, record.id);
        strcpy(record.Keyword, keyword.c_str());
      } else if (price != 0) {
        record.price = price;
      }
      bookFile.update(record, ids[0]);
    }
    return true;
  }

  bool import(const std::string &currentISBN, float quantity, float costPrice) {
    std::vector<int> ids = ISBNIndex.Find(currentISBN);
    if (ids.empty()) {
      // std::cout << "Invalid" << std::endl;
      return false;
    } else {
      BookRecord record;
      bookFile.read(record, ids[0]);
      record.quantity += quantity;
      bookFile.update(record, ids[0]);
    }
    return true;
  }

  bool select(const std::string &ISBN) {
    std::vector<int> ids = ISBNIndex.Find(ISBN);
    if (ids.empty()) {
      return false;
    } else {
      return true;
    }
  }
};

#endif // BOOKSTORAGE_HPP