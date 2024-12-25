#include "FileOperation.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#define BLOCK_SIZE 1000

class BookIndexManager {
private:
  struct Record {
    char index[61];
    int value;
  };

  struct Node {
    Record records[BLOCK_SIZE];
    int curSize;
    int nextPos;
    char firstIndex[61];
    int firstValue;
  };

  const std::string FILE_NAME;

  FileOperation<Node> bookFile;

public:
  BookIndexManager(std::string fileName) : FILE_NAME(fileName) {
    bookFile.initialise(FILE_NAME);
    FileInit();
  }

  void FileInit() {
    if (bookFile.isEmpty()) {
      Node node;
      node.curSize = 0;
      node.nextPos = -1;
      node.firstValue = 0;
      strcpy(node.firstIndex, "");
      for (int i = 0; i < BLOCK_SIZE; i++) {
        node.records[i].value = 0;
        strcpy(node.records[i].index, "");
      }
      bookFile.write(node);
    }
  }

  void PrintAll() {
    Node cur;
    int nodePos = 0;
    while (true) {
      bookFile.read(cur, nodePos);
      for (int i = 0; i < cur.curSize; i++) {
        std::cout << cur.records[i].index << " " << cur.records[i].value
                  << std::endl;
      }
      std::cout << "Node Information: " << nodePos << " " << cur.curSize << " "
                << cur.nextPos << " " << cur.firstIndex << " " << cur.firstValue
                << std::endl;
      std::cout << "Node End" << std::endl;
      if (cur.nextPos == -1)
        break;
      nodePos = cur.nextPos;
    }
  }

  std::vector<int> getAll() {
    Node cur;
    int nodePos = 0;
    std::vector<int> res;
    while (true) {
      bookFile.read(cur, nodePos);
      for (int i = 0; i < cur.curSize; i++) {
        res.push_back(cur.records[i].value);
      }
      if (cur.nextPos == -1)
        break;
      nodePos = cur.nextPos;
    }
    return res;
  }

  bool CompNode(const Node &t1, const std::string &index, int value) {
    int cmp = strcmp(t1.firstIndex, index.c_str());
    return cmp < 0 || (cmp == 0 && t1.firstValue <= value);
  }

  std::pair<Node, int> FindNode(const std::string &index, int value) {
    Node cur, nxt;
    int nodePos = 0;
    std::set<int> visitedPositions;
    
    bookFile.read(cur, 0);
    visitedPositions.insert(0);
    
    if (!CompNode(cur, index, value)) {
      return std::make_pair(cur, nodePos);
    }
    
    while (true) {
      if (cur.nextPos == -1) {
        return std::make_pair(cur, nodePos);
      } else {
        if (!visitedPositions.insert(cur.nextPos).second) {
          return std::make_pair(cur, nodePos);
        }
        
        bookFile.read(nxt, cur.nextPos);
        if (!CompNode(nxt, index, value) && CompNode(cur, index, value)) {
          return std::make_pair(cur, nodePos);
        } else {
          nodePos = cur.nextPos;
          cur = nxt;
        }
      }
    }
  }

  bool Insert(const std::string &index, int value) {
    auto [cur, nodePos] = FindNode(index, value);

    int i = 0;
    while (i < cur.curSize &&
           (strcmp(cur.records[i].index, index.c_str()) < 0 ||
            (strcmp(cur.records[i].index, index.c_str()) == 0 &&
             cur.records[i].value < value))) {
      i++;
    }

    if (i < cur.curSize && strcmp(cur.records[i].index, index.c_str()) == 0 &&
        cur.records[i].value == value) {
      return false;
    }

    for (int j = cur.curSize; j > i; j--) {
      cur.records[j] = cur.records[j - 1];
    }
    strcpy(cur.records[i].index, index.c_str());
    cur.records[i].value = value;
    cur.curSize++;

    strcpy(cur.firstIndex, cur.records[0].index);
    cur.firstValue = cur.records[0].value;

    if (cur.curSize < BLOCK_SIZE) {
      bookFile.update(cur, nodePos);
    } else {
      Node newNode;
      int mid = cur.curSize / 2;
      newNode.curSize = cur.curSize - mid;
      cur.curSize = mid;

      memcpy(newNode.records, cur.records + mid,
             sizeof(Record) * newNode.curSize);

      strcpy(cur.firstIndex, cur.records[0].index);
      cur.firstValue = cur.records[0].value;

      strcpy(newNode.firstIndex, newNode.records[0].index);
      newNode.firstValue = newNode.records[0].value;
      newNode.nextPos = cur.nextPos;
      cur.nextPos = bookFile.write(newNode);

      bookFile.update(cur, nodePos);
    }
    return true;
  }

  void Delete(const std::string &index, int value) {
    auto [cur, nodePos] = FindNode(index, value);

    int i = 0;
    while (i < cur.curSize &&
           !(strcmp(cur.records[i].index, index.c_str()) == 0 &&
             cur.records[i].value == value)) {
      i++;
    }
    if (i == cur.curSize) {
      return;
    }

    for (int j = i; j < cur.curSize - 1; j++) {
      cur.records[j] = cur.records[j + 1];
    }
    cur.curSize--;

    if (cur.curSize > 0) {
      strcpy(cur.firstIndex, cur.records[0].index);
      cur.firstValue = cur.records[0].value;
    } else {
      strcpy(cur.firstIndex, "");
      cur.firstValue = 0;
    }

    bookFile.update(cur, nodePos);

    if (cur.curSize < BLOCK_SIZE / 4 && cur.nextPos != -1) {
      Node nextNode;
      int nextNodePos = cur.nextPos;
      bookFile.read(nextNode, nextNodePos);

      if (cur.curSize + nextNode.curSize < BLOCK_SIZE) {
        memcpy(cur.records + cur.curSize, nextNode.records,
               sizeof(Record) * nextNode.curSize);
        cur.curSize += nextNode.curSize;
        cur.nextPos = nextNode.nextPos;

        strcpy(cur.firstIndex, cur.records[0].index);
        cur.firstValue = cur.records[0].value;

        bookFile.update(cur, nodePos);
        bookFile.Delete(nextNodePos);
      }
    }
    return;
  }

  void Delete(const std::string &index) {
    Node cur;
    int nodePos = 0;
    std::set<int> visitedPositions;
    visitedPositions.insert(0);
    
    bookFile.read(cur, 0);
    while (true) {
      int i = 0;
      while (i < cur.curSize &&
             strcmp(cur.records[i].index, index.c_str()) != 0) {
        i++;
      }
      if (i < cur.curSize) {
        for (int j = i; j < cur.curSize - 1; j++) {
          cur.records[j] = cur.records[j + 1];
        }
        cur.curSize--;
        if (cur.curSize > 0) {
          strcpy(cur.firstIndex, cur.records[0].index);
          cur.firstValue = cur.records[0].value;
        } else {
          strcpy(cur.firstIndex, "");
          cur.firstValue = 0;
        }
        bookFile.update(cur, nodePos);
      }
      if (cur.nextPos == -1) {
        break;
      }
      if (!visitedPositions.insert(cur.nextPos).second) {
        break;
      }
      nodePos = cur.nextPos;
      bookFile.read(cur, nodePos);
    }
  }

  std::vector<int> Find(const std::string &index) {
    Node cur;
    int nodePos = 0;
    std::vector<int> res;
    while (true) {
      bookFile.read(cur, nodePos);
      if (strcmp(cur.firstIndex, index.c_str()) > 0) {
        break;
      }
      for (int i = 0; i < cur.curSize; i++) {
        if (strcmp(cur.records[i].index, index.c_str()) == 0) {
          /*std::cout << cur.records[i].value << " ";*/
          res.push_back(cur.records[i].value);
        }
      }
      if (cur.nextPos == -1)
        break;
      nodePos = cur.nextPos;
    }
    return res;
  }
};