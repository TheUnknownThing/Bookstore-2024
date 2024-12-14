#ifndef BOOKSTORAGE_HPP
#define BOOKSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#define BLOCK_SIZE 600

class Book {
private:
  struct BookRecord {
    char ISBN[21];
    char BookName[61];
    char Keyword[61];
    char Author[61];
    int quantity;
    float price;
  };

  struct Node {
    int curPos; // curPos 指向当前节点在Record文件中的位置
    int curSize;
    int nextPos; // nextPos 指向下一个节点在Node文件中的位置
    char firstIndex[21];
  };

  FileOperation<Node> nodeFile;

  FileOperation<BookRecord[BLOCK_SIZE]> recordFile;

public:
  Book() {
    recordFile.initialise("book.dat");
    nodeFile.initialise("node.dat");
    FileInit();
  }

  void FileInit() {
    if (recordFile.isEmpty()) {
      BookRecord record[BLOCK_SIZE];
      for (int i = 0; i < BLOCK_SIZE; i++) {
        strcpy(record[i].ISBN, "");
      }
      recordFile.write(record);
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      strcpy(node.firstIndex, "");
      nodeFile.write(node);
    }
    if (nodeFile.isEmpty()) {
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      strcpy(node.firstIndex, "");
      nodeFile.write(node);
    }
  }

  void PrintAll() {
    // debug
    BookRecord record[BLOCK_SIZE];
    Node cur;
    int nodePos = 0;
    while (true) {
      nodeFile.read(cur, nodePos);
      recordFile.read(record, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
          std::cout << record[i].ISBN << std::endl;
      }
      std::cout << "Node Information: " << cur.curPos << " " << cur.curSize
                << " " << cur.nextPos << " " << cur.firstIndex << " " << std::endl;
      std::cout << "Node End" << std::endl;
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
  }

  bool CompNode(const Node &t1, const std::string &index) {
    return strcmp(t1.firstIndex, index.c_str()) < 0;
  }

  std::pair<Node, int> FindNode(const std::string &index) {
    Node cur, nxt;
    int nodePos = 0;
    nodeFile.read(cur, nodePos);
    if (!CompNode(cur, index)) {
      return std::make_pair(cur, nodePos);
    }
    while (cur.nextPos != -1) {
      nodeFile.read(nxt, cur.nextPos);
      if (!CompNode(nxt, index)) {
        return std::make_pair(cur, nodePos);
      }
      nodePos = cur.nextPos;
      cur = nxt;
    }
    return std::make_pair(cur, nodePos);
  }

  void Insert(const BookRecord &newBook) {
    auto [cur, nodePos] = FindNode(newBook.ISBN);
    BookRecord record[BLOCK_SIZE];
    recordFile.read(record, cur.curPos);

    int i = 0;
    while (i < cur.curSize && strcmp(record[i].ISBN, newBook.ISBN) < 0) {
      i++;
    }

    if (i < cur.curSize && strcmp(record[i].ISBN, newBook.ISBN) == 0) {
      return;
    }

    for (int j = cur.curSize; j > i; j--) {
      record[j] = record[j - 1];
    }
    record[i] = newBook;
    cur.curSize++;

    strcpy(cur.firstIndex, record[0].ISBN);

    if (cur.curSize < BLOCK_SIZE) {
      recordFile.update(record, cur.curPos);
      nodeFile.update(cur, nodePos);
    } else {
      Node newNode;
      int mid = cur.curSize / 2;
      newNode.curSize = cur.curSize - mid;
      cur.curSize = mid;

      BookRecord newRecord[BLOCK_SIZE];
      memcpy(newRecord, record + mid, sizeof(BookRecord) * newNode.curSize);

      recordFile.update(record, cur.curPos);
      strcpy(cur.firstIndex, record[0].ISBN);

      newNode.curPos = recordFile.write(newRecord);
      strcpy(newNode.firstIndex, newRecord[0].ISBN);
      newNode.nextPos = cur.nextPos;
      cur.nextPos = nodeFile.write(newNode);

      nodeFile.update(cur, nodePos);
    }
  }

  void Delete(const std::string &index) {
    auto [cur, nodePos] = FindNode(index);
    BookRecord record[BLOCK_SIZE];
    recordFile.read(record, cur.curPos);

    int i = 0;
    while (i < cur.curSize && strcmp(record[i].ISBN, index.c_str()) != 0) {
      i++;
    }
    if (i == cur.curSize) {
      return;
    }

    for (int j = i; j < cur.curSize - 1; j++) {
      record[j] = record[j + 1];
    }
    cur.curSize--;

    if (cur.curSize > 0) {
      strcpy(cur.firstIndex, record[0].ISBN);
    } else {
      strcpy(cur.firstIndex, "");
    }

    recordFile.update(record, cur.curPos);
    nodeFile.update(cur, nodePos);

    if (cur.curSize < BLOCK_SIZE / 4 && cur.nextPos != -1) {
      Node nextNode;
      int nextNodePos = cur.nextPos;
      nodeFile.read(nextNode, nextNodePos);

      if (cur.curSize + nextNode.curSize < BLOCK_SIZE) {
        BookRecord nextRecord[BLOCK_SIZE];
        recordFile.read(nextRecord, nextNode.curPos);

        memcpy(record + cur.curSize, nextRecord, sizeof(BookRecord) * nextNode.curSize);
        cur.curSize += nextNode.curSize;
        cur.nextPos = nextNode.nextPos;

        strcpy(cur.firstIndex, record[0].ISBN);

        recordFile.update(record, cur.curPos);
        nodeFile.update(cur, nodePos);

        nodeFile.Delete(nextNodePos);
        recordFile.Delete(nextNode.curPos);
      }
    }
  }

  void Select(const std::string &ISBN) {
    BookRecord record[BLOCK_SIZE];
    Node cur;
    int nodePos = 0;
    bool found = false;

    while (true) {
      nodeFile.read(cur, nodePos);
      recordFile.read(record, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
        if (strcmp(record[i].ISBN, ISBN.c_str()) == 0) {
          found = true;
          break;
        }
      }
      if (found || cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }

    if (!found) {
      BookRecord newRecord;
      strcpy(newRecord.ISBN, ISBN.c_str());
      strcpy(newRecord.BookName, "");
      strcpy(newRecord.Author, "");
      strcpy(newRecord.Keyword, "");
      newRecord.quantity = 0;
      newRecord.price = 0;

      Insert(newRecord);
    }
  }

  void Buy(const std::string &ISBN, int quantity) {
    if (quantity <= 0) {
      std::cout << "Invalid purchase quantity" << std::endl;
      return;
    }
    BookRecord record[BLOCK_SIZE];
    Node cur;
    int nodePos = 0;
    bool found = false;
    int recordIndex = -1;

    while (true) {
      nodeFile.read(cur, nodePos);
      recordFile.read(record, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
        if (strcmp(record[i].ISBN, ISBN.c_str()) == 0) {
          found = true;
          recordIndex = i;
          break;
        }
      }
      if (found || cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }

    if (!found || recordIndex == -1) {
      std::cout << "No matching book" << std::endl;
      return;
    }

    if (record[recordIndex].quantity < quantity) {
      std::cout << "Insufficient stock" << std::endl;
      return;
    }

    record[recordIndex].quantity -= quantity;
    float totalCost = quantity * record[recordIndex].price;

    recordFile.update(record, cur.curPos);

    std::cout << std::fixed << std::setprecision(2) << totalCost << std::endl;
  }

  void Show(const std::string &ISBN = "", const std::string &BookName = "",
            const std::string &Author = "", const std::string &Keyword = "") {
    if (!Keyword.empty() && Keyword.find(' ') != std::string::npos) {
      std::cout << std::endl;
      return;
    }

    bool foundAny = false;
    BookRecord record[BLOCK_SIZE];
    Node cur;
    int nodePos = 0;

    while (true) {
      nodeFile.read(cur, nodePos);
      recordFile.read(record, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
        bool match = true;
        if (!ISBN.empty() && strcmp(record[i].ISBN, ISBN.c_str()) != 0) {
          match = false;
        }
        if (!BookName.empty() && strcmp(record[i].BookName, BookName.c_str()) != 0) {
          match = false;
        }
        if (!Author.empty() && strcmp(record[i].Author, Author.c_str()) != 0) {
          match = false;
        }
        if (!Keyword.empty() && strcmp(record[i].Keyword, Keyword.c_str()) != 0) {
          match = false;
        }

        if (match) {
          std::cout << record[i].ISBN << "\t" << record[i].BookName << "\t"
                    << record[i].Author << "\t" << record[i].Keyword << "\t"
                    << std::fixed << std::setprecision(2)
                    << record[i].price << "\t" << record[i].quantity
                    << "\n";
          foundAny = true;
        }
      }
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
    if (!foundAny) {
      std::cout << std::endl;
    }
  }
};

#endif // BOOKSTORAGE_HPP