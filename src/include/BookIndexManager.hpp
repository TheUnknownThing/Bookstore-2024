#ifndef INDEXMANAGER_HPP
#define INDEXMANAGER_HPP

#include "FileOperation.hpp"
#include <set>
#include <string>
#define BLOCK_SIZE 600

template <typename T> class IndexManager {
private:
  struct IndexNode {
    int curSize;
    int nextPos;
    T firstKey;
    T keys[BLOCK_SIZE];
    std::set<int> recordIds[BLOCK_SIZE];
  };

  FileOperation<IndexNode> indexFile;

public:
  IndexManager(const std::string &filename) {
    indexFile.initialise(filename);
    if (indexFile.isEmpty()) {
      initializeIndex();
    }
  }

  void initializeIndex() {
    IndexNode node;
    node.curSize = 0;
    node.nextPos = -1;
    indexFile.write(node);
  }

  void insert(const T &key, int recordId) {
    auto [node, pos] = findNode(key);

    int i = 0;
    while (i < node.curSize && node.keys[i] < key)
      i++;

    if (i < node.curSize && node.keys[i] == key) {
      node.recordIds[i].insert(recordId);
      indexFile.update(node, pos);
      return;
    }

    for (int j = node.curSize; j > i; j--) {
      node.keys[j] = node.keys[j - 1];
      node.recordIds[j] = node.recordIds[j - 1];
    }

    node.keys[i] = key;
    node.recordIds[i].insert(recordId);
    node.curSize++;

    if (i == 0) {
      node.firstKey = key;
    }

    if (node.curSize < BLOCK_SIZE) {
      indexFile.update(node, pos);
    } else {
      // Split node
      IndexNode newNode;
      int mid = node.curSize / 2;
      newNode.curSize = node.curSize - mid;
      node.curSize = mid;

      for (int j = 0; j < newNode.curSize; j++) {
        newNode.keys[j] = node.keys[mid + j];
        newNode.recordIds[j] = node.recordIds[mid + j];
      }

      newNode.firstKey = newNode.keys[0];
      newNode.nextPos = node.nextPos;
      node.nextPos = indexFile.write(newNode);

      indexFile.update(node, pos);
    }
  }

  void remove(const T &key, int recordId) {
    auto [node, pos] = findNode(key);

    int i = 0;
    while (i < node.curSize && node.keys[i] != key)
      i++;

    if (i == node.curSize)
      return;

    node.recordIds[i].erase(recordId);

    if (node.recordIds[i].empty()) {
      for (int j = i; j < node.curSize - 1; j++) {
        node.keys[j] = node.keys[j + 1];
        node.recordIds[j] = node.recordIds[j + 1];
      }
      node.curSize--;

      if (node.curSize > 0 && i == 0) {
        node.firstKey = node.keys[0];
      }

      if (node.curSize < BLOCK_SIZE / 4 && node.nextPos != -1) {
        IndexNode nextNode;
        indexFile.read(nextNode, node.nextPos);

        if (node.curSize + nextNode.curSize < BLOCK_SIZE) {
          for (int j = 0; j < nextNode.curSize; j++) {
            node.keys[node.curSize + j] = nextNode.keys[j];
            node.recordIds[node.curSize + j] = nextNode.recordIds[j];
          }
          node.curSize += nextNode.curSize;
          node.nextPos = nextNode.nextPos;
          indexFile.update(node, pos);
          indexFile.Delete(node.nextPos);
        }
      }
    }

    indexFile.update(node, pos);
  }

  std::set<int> find(const T &key) {
    auto [node, _] = findNode(key);

    int i = 0;
    while (i < node.curSize && node.keys[i] != key)
      i++;

    if (i == node.curSize)
      return std::set<int>();
    return node.recordIds[i];
  }

private:
  std::pair<IndexNode, int> findNode(const T &key) {
    IndexNode cur;
    int pos = 0;
    indexFile.read(cur, pos);

    while (cur.nextPos != -1 && key >= cur.firstKey) {
      IndexNode next;
      indexFile.read(next, cur.nextPos);
      if (key < next.firstKey)
        break;
      pos = cur.nextPos;
      cur = next;
    }

    return {cur, pos};
  }
};

#endif // INDEXMANAGER_HPP