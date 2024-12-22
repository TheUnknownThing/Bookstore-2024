#ifndef FINANCESTORAGE_HPP
#define FINANCESTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#define BLOCK_SIZE 600

class Finance {
private:
  float totalIncome = 0;
  float totalOutcome = 0;
  int lastId = 0;

  struct FinanceRecord {
    int id;
    float income;
    float outcome;
  };

  struct Node {
    FinanceRecord records[BLOCK_SIZE];
    int curSize;
    int nextPos;
    int firstId;
  };

  FileOperation<Node> financeFile;

public:
  Finance() {
    financeFile.initialise("finance.dat");
    FileInit();
  }

  void FileInit() {
    if (financeFile.isEmpty()) {
      Node node;
      node.curSize = 0;
      node.nextPos = -1;
      node.firstId = 0;
      for (int i = 0; i < BLOCK_SIZE; i++) {
        node.records[i].id = 0;
        node.records[i].income = 0;
        node.records[i].outcome = 0;
      }
      financeFile.write(node);
    }

    Node cur;
    int nodePos = 0;
    while (true) {
      financeFile.read(cur, nodePos);
      for (int i = 0; i < cur.curSize; i++) {
        lastId = std::max(lastId, cur.records[i].id);
        totalIncome += cur.records[i].income;
        totalOutcome += cur.records[i].outcome;
      }
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
  }

  float GetTotalIncome() { return totalIncome; }

  float GetTotalOutcome() { return totalOutcome; }

  void AddRecord(float income, float outcome) {
    Node cur;
    int nodePos = 0;

    while (true) {
      financeFile.read(cur, nodePos);
      if (cur.curSize < BLOCK_SIZE) {
        break;
      }
      if (cur.nextPos == -1) {
        Node newNode;
        newNode.curSize = 0;
        newNode.nextPos = -1;
        newNode.firstId = lastId + 1;
        cur.nextPos = financeFile.write(newNode);
        financeFile.update(cur, nodePos);
        nodePos = cur.nextPos;
        financeFile.read(cur, nodePos);
        break;
      }
      nodePos = cur.nextPos;
    }

    lastId++;
    cur.records[cur.curSize].id = lastId;
    cur.records[cur.curSize].income = income;
    cur.records[cur.curSize].outcome = outcome;
    if (cur.curSize == 0) {
      cur.firstId = lastId;
    }
    cur.curSize++;

    totalIncome += income;
    totalOutcome += outcome;

    financeFile.update(cur, nodePos);
  }

  void AddOutcome(float outcome) { AddRecord(0, outcome); }

  void AddIncome(float income) { AddRecord(income, 0); }

  std::pair<float, float> GetLastNSum(int n) {
    float sumIncome = 0, sumOutcome = 0;
    int targetId = lastId - n + 1;

    Node cur;
    int nodePos = 0;
    while (nodePos != -1) {
      financeFile.read(cur, nodePos);
      if (cur.firstId + cur.curSize - 1 < targetId) {
        nodePos = cur.nextPos;
        continue;
      }

      for (int i = cur.curSize - 1; i >= 0; i--) {
        if (cur.records[i].id >= targetId) {
          sumIncome += cur.records[i].income;
          sumOutcome += cur.records[i].outcome;
        } else {
          return {sumIncome, sumOutcome};
        }
      }
      nodePos = cur.nextPos;
    }
    return {sumIncome, sumOutcome};
  }
};

#endif // FINANCESTORAGE_HPP