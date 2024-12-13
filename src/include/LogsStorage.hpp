#ifndef LOGSSTORAGE_HPP
#define LOGSSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>

class Logs {
private:
  enum OperationType {
    LOGIN,
    LOGOUT,
    USERADD,
    REGISTER,
    PASSWD,
    DELETE,
    SHOW,
    BUY,
    SELECT,
    MODIFY,
    IMPORT,
    SHOWFINANCE,
    REPORTFINANCE,
    REPORTEMPLOYEE,
    LOG
  };

  struct FinancialLog {
    int income;
    int outcome;
  };

  struct BookLog {
    char ISBN[21];
    int quantity;
    int costPrice;
    int sellingPrice;
  };

  struct UserLog {
    char userID[31];
    char userName[31];
    int privilege;
  };

  struct Log {
    OperationType op;
    char userID[31];
    union {
      FinancialLog financialLog;
      BookLog bookLog;
      UserLog userLog;
    };
  };

  FileOperation<Log> logFile;

  int logCount = 0;

public:
  Logs() { logFile.initialise("log.dat"); }

  void LogAdd(OperationType op, const std::string &userID,
              const BookLog &bookLog) {
    Log log;
    log.op = op;
    strcpy(log.userID, userID.c_str());
    log.bookLog = bookLog;
    logFile.write(log);
    logCount++;
  }

  void LogAdd(OperationType op, const std::string &userID,
              const UserLog &userLog) {
    Log log;
    log.op = op;
    strcpy(log.userID, userID.c_str());
    log.userLog = userLog;
    logFile.write(log);
    logCount++;
  }

  void LogAdd(OperationType op, const std::string &userID,
              const FinancialLog &financialLog) {
    Log log;
    log.op = op;
    strcpy(log.userID, userID.c_str());
    log.financialLog = financialLog;
    logFile.write(log);
    logCount++;
  }

  void ShowLog() {
    Log log;
    int logPos = 0;
    for (int i = 0; i < logCount; i++) {
      logFile.read(log, logPos);
      std::cout << i + 1 << " ";
      if (log.op == LOGIN) {
        std::cout << "User " << log.userID << " log in" << std::endl;
      } else if (log.op == LOGOUT) {
        std::cout << "User " << log.userID << " log out" << std::endl;
      } else if (log.op == USERADD) {
        std::cout << "User " << log.userID << " add user " << log.userLog.userID
                  << std::endl;
      } else if (log.op == REGISTER) {
        std::cout << "User " << log.userID << " register user "
                  << log.userLog.userID << std::endl;
      } else if (log.op == PASSWD) {
        std::cout << "User " << log.userID << " change password of user "
                  << log.userLog.userID << std::endl;
      } else if (log.op == DELETE) {
        std::cout << "User " << log.userID << " delete user "
                  << log.userLog.userID << std::endl;
      } else if (log.op == SHOW) {
        std::cout << "User " << log.userID << " show user " << log.userLog.userID
                  << std::endl;
      } else if (log.op == BUY) {
        std::cout << "User " << log.userID << " buy book " << log.bookLog.ISBN
                  << std::endl;
      } else if (log.op == SELECT) {
        std::cout << "User " << log.userID << " select book "
                  << log.bookLog.ISBN << std::endl;
      } else if (log.op == MODIFY) {
        std::cout << "User " << log.userID << " modify book "
                  << log.bookLog.ISBN << std::endl;
      } else if (log.op == IMPORT) {
        std::cout << "User " << log.userID << " import book "
                  << log.bookLog.ISBN << std::endl;
      } else if (log.op == SHOWFINANCE) {
        std::cout << "User " << log.userID << " show finance" << std::endl;
      } else if (log.op == REPORTFINANCE) {
        std::cout << "User " << log.userID << " report finance" << std::endl;
      } else if (log.op == REPORTEMPLOYEE) {
        std::cout << "User " << log.userID << " report employee" << std::endl;
      } else if (log.op == LOG) {
        std::cout << "User " << log.userID << " get log" << std::endl;
      }
      logPos+=sizeof(Log);
    }
  }
};

#endif // LOGSSTORAGE_HPP