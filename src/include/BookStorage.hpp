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
    float sellingPrice;
    int value;
  };

  struct Node {
    int curPos; // curPos 指向当前节点在Record文件中的位置
    int curSize;
    int nextPos; // nextPos 指向下一个节点在Node文件中的位置
    int firstValue;
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
        record[i].value = 0;
        strcpy(record[i].ISBN, "");
      }
      recordFile.write(record);
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      node.firstValue = 0;
      strcpy(node.firstIndex, "");
      nodeFile.write(node);
    }
    if (nodeFile.isEmpty()) {
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      node.firstValue = 0;
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
        if (record[i].value != 0) {
          std::cout << record[i].ISBN << " " << record[i].value << std::endl;
        }
      }
      std::cout << "Node Information: " << cur.curPos << " " << cur.curSize
                << " " << cur.nextPos << " " << cur.firstIndex << " "
                << cur.firstValue << std::endl;
      std::cout << "Node End" << std::endl;
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
  }

  bool CompNode(const Node &t1, const std::string &index, int value) {
    int cmp = strcmp(t1.firstIndex, index.c_str());
    return cmp < 0 ||
           (cmp == 0 && t1.firstValue <= value); // 返回Node < index, value
  }

  std::pair<Node, int> FindNode(const std::string &index, int value) {
    Node cur, nxt;
    int nodePos = 0;
    nodeFile.read(cur, 0);
    if (!CompNode(cur, index, value)) {
      return std::make_pair(cur, nodePos);
    }
    while (true) {
      if (cur.nextPos == -1) {
        // return cur;
        return std::make_pair(cur, nodePos);
      } else {
        nodeFile.read(nxt, cur.nextPos);
        if (!CompNode(nxt, index, value) && CompNode(cur, index, value)) {
          // return cur;
          return std::make_pair(cur, nodePos);
        } else {
          nodePos = cur.nextPos;
          cur = nxt;
        }
      }
    }
  }

  void Insert(const std::string &index, int value) {
    // Node cur = FindNode(index, value);
    auto [cur, nodePos] = FindNode(index, value);
    BookRecord record[BLOCK_SIZE];
    recordFile.read(record, cur.curPos);

    int i = 0;
    while (i < cur.curSize && (strcmp(record[i].ISBN, index.c_str()) < 0 ||
                               (strcmp(record[i].ISBN, index.c_str()) == 0 &&
                                record[i].value < value))) {
      i++;
    }

    // if same index and value, do nothing
    if (i < cur.curSize && strcmp(record[i].ISBN, index.c_str()) == 0 &&
        record[i].value == value) {
      return;
    }

    for (int j = cur.curSize; j > i; j--) {
      record[j] = record[j - 1];
    }
    strcpy(record[i].ISBN, index.c_str());
    record[i].value = value;
    cur.curSize++;

    strcpy(cur.firstIndex, record[0].ISBN);
    cur.firstValue = record[0].value;

    if (cur.curSize < BLOCK_SIZE) {
      recordFile.update(record, cur.curPos);
      nodeFile.update(cur, nodePos);
    } else {
      // Split the node
      // std::cout << "Split" << std::endl; // debug
      Node newNode;
      int mid = cur.curSize / 2;
      newNode.curSize = cur.curSize - mid;
      cur.curSize = mid;

      BookRecord newRecord[BLOCK_SIZE];
      memcpy(newRecord, record + mid, sizeof(BookRecord) * newNode.curSize);

      // Update the current node
      recordFile.update(record, cur.curPos);
      strcpy(cur.firstIndex, record[0].ISBN);
      cur.firstValue = record[0].value;

      // Write the new node
      newNode.curPos = recordFile.write(newRecord);
      strcpy(newNode.firstIndex, newRecord[0].ISBN);
      newNode.firstValue = newRecord[0].value;
      newNode.nextPos = cur.nextPos;
      cur.nextPos = nodeFile.write(newNode);

      nodeFile.update(cur, nodePos);
    }
  }

  void Delete(const std::string &index, int value) {
    auto [cur, nodePos] = FindNode(index, value);
    BookRecord record[BLOCK_SIZE];
    recordFile.read(record, cur.curPos);

    int i = 0;
    while (i < cur.curSize && !(strcmp(record[i].ISBN, index.c_str()) == 0 &&
                                record[i].value == value)) {
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
      cur.firstValue = record[0].value;
    } else {
      strcpy(cur.firstIndex, "");
      cur.firstValue = 0;
    }

    recordFile.update(record, cur.curPos);
    nodeFile.update(cur, nodePos);

    // Check if merge is needed
    /*if (cur.curSize < BLOCK_SIZE / 4 && cur.nextPos != -1) {
      Node nextNode;
      int nextNodePos = cur.nextPos; // Store nextNode's position
      nodeFile.read(nextNode, nextNodePos);

      if (cur.curSize + nextNode.curSize < BLOCK_SIZE) {
        Record nextRecord[BLOCK_SIZE];
        recordFile.read(nextRecord, nextNode.curPos);

        memcpy(record + cur.curSize, nextRecord,
               sizeof(Record) * nextNode.curSize);
        cur.curSize += nextNode.curSize;
        cur.nextPos = nextNode.nextPos; // Update cur's nextPos

        strcpy(cur.firstIndex, record[0].index);
        cur.firstValue = record[0].value;

        recordFile.update(record, cur.curPos);
        nodeFile.update(cur, nodePos);

        // Delete nextNode from files
        nodeFile.Delete(nextNodePos);
        recordFile.Delete(nextNode.curPos);
      }
    }*/
    if (cur.curSize == 0) {
      Node nextNode;
      int nextNodePos = cur.nextPos;
      nodeFile.read(nextNode, nextNodePos);
      cur.nextPos = nextNode.nextPos;
      strcpy(cur.firstIndex, nextNode.firstIndex);
      cur.firstValue = nextNode.firstValue;
      nodeFile.update(cur, nodePos);
      nodeFile.Delete(nextNodePos);
    }
  }

  void Find(const std::string &index) {
    Node cur;
    int nodePos = 0;
    bool found = false;
    while (true) {
      nodeFile.read(cur, nodePos);
      if (strcmp(cur.firstIndex, index.c_str()) > 0) {
        break;
      }
      BookRecord record[BLOCK_SIZE];
      recordFile.read(record, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
        if (strcmp(record[i].ISBN, index.c_str()) == 0) {
          std::cout << record[i].value << " ";
          found = true;
        }
      }
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
    if (!found) {
      std::cout << "null" << std::endl;
    } else {
      std::cout << std::endl;
    }
  }

  void Select(const std::string &ISBN) {
    // Search for the book with the specified ISBN
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
      // Create a new record with only ISBN
      BookRecord newRecord;
      strcpy(newRecord.ISBN, ISBN.c_str());
      strcpy(newRecord.BookName, "");
      strcpy(newRecord.Author, "");
      strcpy(newRecord.Keyword, "");
      newRecord.quantity = 0;
      newRecord.sellingPrice = 0.0f;
      newRecord.value = 0;
      // Insert the new record using existing logic
      Insert(newRecord.ISBN, newRecord.value);
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
    float totalCost = quantity * record[recordIndex].sellingPrice;

    recordFile.update(record, cur.curPos);

    std::cout << std::fixed << std::setprecision(2) << totalCost << std::endl;
  }

  void Show(const std::string &ISBN = "", const std::string &BookName = "",
            const std::string &Author = "", const std::string &Keyword = "") {
    if (!Keyword.empty() && Keyword.find(' ') != std::string::npos) {
      // Operation fails if Keyword contains multiple words
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
        if (!BookName.empty() &&
            strcmp(record[i].BookName, BookName.c_str()) != 0) {
          match = false;
        }
        if (!Author.empty() && strcmp(record[i].Author, Author.c_str()) != 0) {
          match = false;
        }
        if (!Keyword.empty() &&
            strcmp(record[i].Keyword, Keyword.c_str()) != 0) {
          match = false;
        }

        if (match) {
          std::cout << record[i].ISBN << "\t" << record[i].BookName << "\t"
                    << record[i].Author << "\t" << record[i].Keyword << "\t"
                    << std::fixed << std::setprecision(2)
                    << record[i].sellingPrice << "\t" << record[i].quantity
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