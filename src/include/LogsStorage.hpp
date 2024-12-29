#ifndef LOGSSTORAGE_HPP
#define LOGSSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>

class Logs {
public:
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

  Logs() {
    FileInit();
  }

  void AddLoginLog(const std::string &userID) {
    Log log;
    log.op = LOGIN;
    strncpy(log.userID, userID.c_str(), 30);
    log.userID[30] = '\0';
    AddLog(log);
  }

  void AddLogoutLog(const std::string &userID) {
    Log log;
    log.op = LOGOUT;
    strncpy(log.userID, userID.c_str(), 30);
    log.userID[30] = '\0';
    AddLog(log);
  }

  void AddUserLog(OperationType op, const std::string &currentUser,
                 const std::string &targetUser, const std::string &userName, int privilege) {
    Log log;
    log.op = op;
    strncpy(log.userID, currentUser.c_str(), 30);
    log.userID[30] = '\0';
    strncpy(log.userLog.userID, targetUser.c_str(), 30);
    strncpy(log.userLog.userName, userName.c_str(), 30);
    log.userLog.privilege = privilege;
    AddLog(log);
  }

  void AddBookLog(OperationType op, const std::string &userID,
                 const std::string &ISBN, int quantity, double price) {
    Log log;
    log.op = op;
    strncpy(log.userID, userID.c_str(), 30);
    log.userID[30] = '\0';
    strncpy(log.bookLog.ISBN, ISBN.c_str(), 20);
    log.bookLog.ISBN[20] = '\0';
    log.bookLog.quantity = quantity;
    if (op == BUY) {
      log.bookLog.sellingPrice = price;
    } else {
      log.bookLog.costPrice = price;
    }
    AddLog(log);
  }

  void AddModifyLog(const std::string &userID, const std::string &ISBN) {
    Log log;
    log.op = MODIFY;
    strncpy(log.userID, userID.c_str(), 30);
    log.userID[30] = '\0';
    strncpy(log.bookLog.ISBN, ISBN.c_str(), 20);
    log.bookLog.ISBN[20] = '\0';
    AddLog(log);
  }

private:
  struct Node {
    Log log;
    int nextPos;
  };

  FileOperation<Node> file;
  int head = -1;

  void FileInit() {
    file.initialise("logs.dat");
    if (!file.isEmpty()) {
      Node firstNode;
      file.read(firstNode, 0);
      head = 0;
    }
  }

  void AddLog(const Log &newLog) {
    Node newNode;
    newNode.log = newLog;
    newNode.nextPos = head;
    head = file.write(newNode);
  }

public:
  void ShowLog() {
    if (head == -1) {
      std::cout << "No logs available." << std::endl;
      return;
    }

    Node currentNode;
    int currentPos = head;

    std::cout << "\n=== System Log Report ===\n" << std::endl;

    while (currentPos != -1) {
      file.read(currentNode, currentPos);
      
      const Log &log = currentNode.log;
      std::cout << "User: " << log.userID << " | Operation: ";
      
      switch (log.op) {
        case LOGIN:
          std::cout << "Login";
          break;
        case LOGOUT:
          std::cout << "Logout";
          break;
        case BUY:
          std::cout << "Buy - ISBN: " << log.bookLog.ISBN 
                    << ", Quantity: " << log.bookLog.quantity
                    << ", Price: " << std::fixed << std::setprecision(2) 
                    << log.bookLog.sellingPrice;
          break;
        case IMPORT:
          std::cout << "Import - ISBN: " << log.bookLog.ISBN
                    << ", Quantity: " << log.bookLog.quantity
                    << ", Cost: " << std::fixed << std::setprecision(2)
                    << log.bookLog.costPrice;
          break;
        case MODIFY:
          std::cout << "Modify - ISBN: " << log.bookLog.ISBN;
          break;
        case USERADD:
        case REGISTER:
          std::cout << (log.op == USERADD ? "Add User" : "Register")
                    << " - ID: " << log.userLog.userID
                    << ", Name: " << log.userLog.userName
                    << ", Privilege: " << log.userLog.privilege;
          break;
        default:
          std::cout << "Other Operation";
      }
      std::cout << std::endl;

      currentPos = currentNode.nextPos;
    }
    std::cout << "\n=== End of Log Report ===\n" << std::endl;
  }

  void ShowEmployeeLog() {
    // sort by user ID, record USERADD, IMPORT, MODIFY

    if (head == -1) {
      std::cout << "No logs available." << std::endl;
      return;
    }
    
    Node currentNode;
    int currentPos = head;

    std::vector<Node> logs;

    while (currentPos != -1) {
      file.read(currentNode, currentPos);
      if (currentNode.log.op == USERADD || currentNode.log.op == IMPORT || currentNode.log.op == MODIFY && strcmp(currentNode.log.userID, "root") != 0) {
        logs.push_back(currentNode);
      }
      currentPos = currentNode.nextPos;
    }

    std::sort(logs.begin(), logs.end(), [](const Node &a, const Node &b) {
      return strcmp(a.log.userID, b.log.userID) < 0;
    });

    std::cout << "\n=== Employee Log Report ===\n" << std::endl;

    for (const auto &node : logs) {
      const Log &log = node.log;
      std::cout << "User: " << log.userID << " | Operation: ";
      
      switch (log.op) {
        case USERADD:
          std::cout << "Add User - ID: " << log.userLog.userID
                    << ", Name: " << log.userLog.userName
                    << ", Privilege: " << log.userLog.privilege;
          break;
        case IMPORT:
          std::cout << "Import - ISBN: " << log.bookLog.ISBN
                    << ", Quantity: " << log.bookLog.quantity
                    << ", Cost: " << std::fixed << std::setprecision(2)
                    << log.bookLog.costPrice;
          break;
        case MODIFY:
          std::cout << "Modify - ISBN: " << log.bookLog.ISBN;
          break;
        default:
          std::cout << "Other Operation";
      }
      std::cout << std::endl;
    }
  }
};

#endif // LOGSSTORAGE_HPP