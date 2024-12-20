#ifndef BOOKINDEXMANAGER_HPP
#define BOOKINDEXMANAGER_HPP

#include "FileOperation.hpp"
#include <string>
#include <vector>
#define BLOCK_SIZE 600

template <typename T> class BookIndexManager {
private:
  struct IndexNode {
    int curSize;
    int nextPos; // 指向下一个节点的位置
    T firstKey;
    std::pair<T, int> keys[BLOCK_SIZE];
  };

  FileOperation<IndexNode> indexFile;

public:

  BookIndexManager(const std::string &filename) : indexFile(filename) {
    if (indexFile.isEmpty()) {
      IndexNode dummy;
      dummy.curSize = 0;
      dummy.nextPos = -1;
      indexFile.write(dummy);
    }
  }

  bool compNode(const IndexNode &node, const T &key) {
    return node.firstKey < key;
  }

  std::pair<IndexNode, int> findNode(const T &key) {
    IndexNode curNode, nextNode;
    int nodePos = 0;
    indexFile.read(curNode, nodePos);
    while (true) {
      if (!compNode(curNode, key)) {
        return {curNode, nodePos};
      }
      if (curNode.nextPos == -1) {
        return {curNode, nodePos};
      }
      nodePos = curNode.nextPos;
      indexFile.read(curNode, nodePos);
    }
  }

  void insert(const T &key, int value) {
    auto [curNode, nodePos] = findNode(key);
    if (curNode.curSize < BLOCK_SIZE) {
      int i = curNode.curSize - 1;
      while (i >= 0 && curNode.keys[i].first > key) {
        curNode.keys[i + 1] = curNode.keys[i];
        i--;
      }
      curNode.keys[i + 1] = {key, value};
      curNode.curSize++;
      indexFile.update(curNode, nodePos);
    } else {
      IndexNode newNode;
      newNode.curSize = 0;
      newNode.nextPos = curNode.nextPos;
      curNode.nextPos = indexFile.write(newNode);
      indexFile.update(curNode, nodePos);

      T midKey = curNode.keys[BLOCK_SIZE / 2].first;
      for (int i = BLOCK_SIZE / 2 + 1; i < BLOCK_SIZE; i++) {
        newNode.keys[newNode.curSize++] = curNode.keys[i];
      }
      curNode.curSize = BLOCK_SIZE / 2;
      indexFile.update(curNode, nodePos);

      if (compNode(curNode, key)) {
        indexFile.write(newNode);
      } else {
        indexFile.update(newNode, nodePos);
      }
    }
  }

  void remove(const T &key, int value) {
    auto [curNode, nodePos] = findNode(key);
    int i = 0;
    while (i < curNode.curSize && curNode.keys[i].first != key) {
      i++;
    }
    if (i == curNode.curSize) {
      return;
    }
    for (int j = i; j < curNode.curSize - 1; j++) {
      curNode.keys[j] = curNode.keys[j + 1];
    }
    curNode.curSize--;
    indexFile.update(curNode, nodePos);

    if (curNode.curSize < BLOCK_SIZE / 4 && curNode.nextPos != -1) {
      IndexNode nextNode;
      int nextNodePos = curNode.nextPos;
      indexFile.read(nextNode, nextNodePos);
      if (curNode.curSize + nextNode.curSize < BLOCK_SIZE) {
        T midKey = nextNode.keys[0].first;
        for (int i = 0; i < nextNode.curSize; i++) {
          curNode.keys[curNode.curSize++] = nextNode.keys[i];
        }
        curNode.nextPos = nextNode.nextPos;
        indexFile.update(curNode, nodePos);
        indexFile.Delete(nextNodePos);
      }
    }
  }

  std::vector<int> find(const T &key) {
    std::vector<int> results;
    auto [curNode, nodePos] = findNode(key);
    for (int i = 0; i < curNode.curSize; i++) {
      if (curNode.keys[i].first == key) {
        results.push_back(curNode.keys[i].second);
      }
    }
    return results;
  }

};

#endif // BOOKINDEXMANAGER_HPP