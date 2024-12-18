#ifndef BOOKSTORAGE_HPP
#define BOOKSTORAGE_HPP

#include "BookIndexManager.hpp"
#include "FileOperation.hpp"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#define BLOCK_SIZE 600

class Book {
private:
  struct BookRecord {
    int id;
    char ISBN[21];
    char BookName[61];
    char Keyword[61];
    char Author[61];
    int quantity;
    float price;
  };

  template <typename T> struct IndexNode {
    int curPos;
    int curSize;
    int nextPos;
    T firstKey;
    std::map<T, std::set<int>> indexMap;
  };

  IndexManager<std::string> ISBNIndex =
      IndexManager<std::string>("ISBNIndex.dat");
  IndexManager<std::string> BookNameIndex =
      IndexManager<std::string>("BookNameIndex.dat");
  IndexManager<std::string> KeywordIndex =
      IndexManager<std::string>("KeywordIndex.dat");
  IndexManager<std::string> AuthorIndex =
      IndexManager<std::string>("AuthorIndex.dat");
  IndexManager<float> PriceIndex = IndexManager<float>("PriceIndex.dat");
  FileOperation<BookRecord> bookFile;

  int nextId;

public:
  Book() {
    bookFile.initialise("book.dat");
    ISBNIndex.initializeIndex();
    BookNameIndex.initializeIndex();
    KeywordIndex.initializeIndex();
    AuthorIndex.initializeIndex();
    PriceIndex.initializeIndex();
    nextId = 0;
  }

  
private:
  
};

#endif // BOOKSTORAGE_HPP