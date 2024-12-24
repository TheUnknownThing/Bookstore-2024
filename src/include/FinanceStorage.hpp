#ifndef FINANCESTORAGE_HPP
#define FINANCESTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

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

  FileOperation<FinanceRecord> financeFile;

public:
  Finance() {
    financeFile.initialise("finance.dat");
    FileInit();
  }

  void FileInit() {
    if (financeFile.isEmpty()) {
      FinanceRecord record;
      record.id = 0;
      record.income = 0;
      record.outcome = 0;
      financeFile.write(record);
    }

    FinanceRecord lastRecord;
    financeFile.getLast(lastRecord);
    lastId = lastRecord.id;
    totalIncome = lastRecord.income;
    totalOutcome = lastRecord.outcome;
  }

  double GetTotalIncome() { return totalIncome / 100.0; }

  double GetTotalOutcome() { return totalOutcome / 100.0; }

  void AddRecord(double income, double outcome) {
    long long intIncome = round(income * 100);
    long long intOutcome = round(outcome * 100);

    totalIncome += intIncome;
    totalOutcome += intOutcome;
    lastId++;

    FinanceRecord newRecord;
    newRecord.id = lastId;
    newRecord.income = totalIncome;
    newRecord.outcome = totalOutcome;

    financeFile.write(newRecord);
  }

  void AddOutcome(double outcome) { AddRecord(0, outcome); }

  void AddIncome(double income) { AddRecord(income, 0); }

  std::pair<double, double> GetLastNSum(int n) {
    long long sumIncome = 0, sumOutcome = 0;
    int targetId = lastId - n;

    if (targetId < 0) {
      return {-1, -1};
    }

    FinanceRecord cur;
    int pos = sizeof(FinanceRecord) * targetId;

    financeFile.read(cur, pos);

    if (cur.id == targetId) {
        sumIncome = totalIncome - cur.income;
        sumOutcome = totalOutcome - cur.outcome;
    } else {
        sumIncome = totalIncome - cur.income;
        sumOutcome = totalOutcome - cur.outcome;
    }

    return {sumIncome / 100.0, sumOutcome / 100.0};
  }
};

#endif // FINANCESTORAGE_HPP