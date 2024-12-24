#ifndef FINANCESTORAGE_HPP
#define FINANCESTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#define BLOCK_SIZE 1000

class Finance {
private:
  long long totalIncome = 0;
  long long totalOutcome = 0;
  int lastId = 0;

  struct FinanceRecord {
    int id;
    long long income;
    long long outcome;
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
      if (cur.nextPos == -1)
        break;
      nodePos = cur.nextPos;
    }
  }

  double GetTotalIncome() { return totalIncome / 100.0; }

  double GetTotalOutcome() { return totalOutcome / 100.0; }

  void AddRecord(double income, double outcome) {
    long long intIncome = round(income * 100);
    long long intOutcome = round(outcome * 100);

    Node cur;
    int nodePos = 0;

    while (true) {
      financeFile.read(cur, nodePos);
      if (cur.curSize < BLOCK_SIZE)
        break;
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

    totalIncome += intIncome;
    totalOutcome += intOutcome;

    lastId++;
    cur.records[cur.curSize].id = lastId;
    cur.records[cur.curSize].income = totalIncome;
    cur.records[cur.curSize].outcome = totalOutcome;
    if (cur.curSize == 0) {
      cur.firstId = lastId;
    }
    cur.curSize++;

    financeFile.update(cur, nodePos);
  }

  void AddOutcome(double outcome) { AddRecord(0, outcome); }

  void AddIncome(double income) { AddRecord(income, 0); }

  std::pair<double, double> GetLastNSum(int n) {
    long long sumIncome = totalIncome, sumOutcome = totalOutcome;
    int targetId = lastId - n;

    if (targetId < 0) {
      return {-1, -1};
    }

    Node cur;
    int nodePos = 0;
    while (nodePos != -1) {
      financeFile.read(cur, nodePos);
      if (cur.firstId + cur.curSize - 1 < targetId) {
        nodePos = cur.nextPos;
        continue;
      }

      for (int i = cur.curSize - 1; i >= 0; i--) {
        if (cur.records[i].id == targetId) {
          sumIncome -= cur.records[i].income;
          sumOutcome -= cur.records[i].outcome;
          break;
        }
      }
      nodePos = cur.nextPos;
    }
    return {sumIncome / 100.0, sumOutcome / 100.0};
  }
};

#endif // FINANCESTORAGE_HPP