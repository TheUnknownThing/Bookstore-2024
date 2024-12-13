#ifndef FINANCESTORAGE_HPP
#define FINANCESTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>

class Finance {
private:
  //TODO
  int totalIncome;
  int totalOutcome;

  FileOperation<int> financeFile;

public:
  int GetTotalIncome() {
    return totalIncome;
  }

  int GetTotalOutcome() {
    return totalOutcome;
  }

  void AddTotalIncome(int income) {
    totalIncome += income;
  }

  void AddTotalOutcome(int outcome) {
    totalOutcome += outcome;
  }

  void FileInit() {
    //TODO
  }



};

#endif // FINANCESTORAGE_HPP