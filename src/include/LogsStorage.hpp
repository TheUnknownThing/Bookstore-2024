#ifndef LOGSSTORAGE_HPP
#define LOGSSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>

#define BLOCK_SIZE 600

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

  struct Record {
    Log log;
  };

  struct Node {
    int curPos;
    int curSize;
    int nextPos;
    Log firstLog;
  };

  FileOperation<Node> nodeFile;
  FileOperation<Record[BLOCK_SIZE]> recordFile;

public:
  Logs() {
    recordFile.initialise("log.dat");
    nodeFile.initialise("logNode.dat");
    FileInit();
  }

  void FileInit() {
    if (recordFile.isEmpty()) {
      Record records[BLOCK_SIZE];
      memset(records, 0, sizeof(records));
      recordFile.write(records);
    }
    if (nodeFile.isEmpty()) {
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      memset(&node.firstLog, 0, sizeof(Log));
      nodeFile.write(node);
    }
  }



  void AddLog(const Log &newLog) {
    Node curNode;
    int nodePos = 0;
    nodeFile.read(curNode, nodePos);

    while (curNode.nextPos != -1) {
      nodePos = curNode.nextPos;
      nodeFile.read(curNode, nodePos);
    }

    Record records[BLOCK_SIZE];
    recordFile.read(records, curNode.curPos);

    if (curNode.curSize < BLOCK_SIZE) {
      records[curNode.curSize].log = newLog;
      curNode.curSize++;

      if (curNode.curSize == 1) {
        curNode.firstLog = newLog;
      }

      recordFile.update(records, curNode.curPos);
      nodeFile.update(curNode, nodePos);
    } else {
      Node newNode;
      Record newRecords[BLOCK_SIZE];
      memset(newRecords, 0, sizeof(newRecords));

      newRecords[0].log = newLog;
      newNode.curPos = recordFile.write(newRecords);
      newNode.curSize = 1;
      newNode.nextPos = -1;
      newNode.firstLog = newLog;

      curNode.nextPos = nodeFile.write(newNode);
      nodeFile.update(curNode, nodePos);
    }
  }

 void ShowLog() {
    Node curNode;
    int nodePos = 0;
    nodeFile.read(curNode, nodePos);

    int logIndex = 1;

    while (true) {
      Record records[BLOCK_SIZE];
      recordFile.read(records, curNode.curPos);

      for (int i = 0; i < curNode.curSize; ++i) {
        const Log &log = records[i].log;
        std::cout << logIndex++ << " ";
        if (log.op == LOGIN) {
          std::cout << "User " << log.userID << " log in" << std::endl;
        } else if (log.op == LOGOUT) {
          std::cout << "User " << log.userID << " log out" << std::endl;
        } else if (log.op == USERADD) {
          std::cout << "User " << log.userID << " add user " << log.userLog.userID << " with privilege " << log.userLog.privilege << std::endl;
        } else if (log.op == REGISTER) {
          std::cout << "User " << log.userID << " register user " << log.userLog.userID << " with privilege " << log.userLog.privilege << std::endl;
        } else if (log.op == PASSWD) {
          std::cout << "User " << log.userID << " change password of user " << log.userLog.userID << std::endl;
        } else if (log.op == DELETE) {
          std::cout << "User " << log.userID << " delete user " << log.userLog.userID << std::endl;
        } else if (log.op == SHOW) {
          std::cout << "User " << log.userID << " show book" << std::endl;
        } else if (log.op == BUY) {
          std::cout << "User " << log.userID << " buy book " << log.bookLog.ISBN << " with quantity " << log.bookLog.quantity << std::endl;
        } else if (log.op == SELECT) {
          std::cout << "User " << log.userID << " select book " << log.bookLog.ISBN << std::endl;
        } else if (log.op == MODIFY) {
          std::cout << "User " << log.userID << " modify book " << log.bookLog.ISBN << " with quantity " << log.bookLog.quantity << " cost price " << log.bookLog.costPrice << " selling price " << log.bookLog.sellingPrice << std::endl;
        } else if (log.op == IMPORT) {
          std::cout << "User " << log.userID << " import book " << log.bookLog.ISBN << " with quantity " << log.bookLog.quantity << " cost price " << log.bookLog.costPrice << std::endl;
        } else if (log.op == SHOWFINANCE) {
          std::cout << "User " << log.userID << " show finance" << std::endl;
        } else if (log.op == REPORTFINANCE) {
          std::cout << "User " << log.userID << " report finance" << std::endl;
        } else if (log.op == REPORTEMPLOYEE) {
          std::cout << "User " << log.userID << " report employee" << std::endl;
        } else if (log.op == LOG) {
          std::cout << "User " << log.userID << " log" << std::endl;
        }
      }

      if (curNode.nextPos == -1) {
        break;
      } else {
        nodePos = curNode.nextPos;
        nodeFile.read(curNode, nodePos);
      }
    }
  }
};

#endif // LOGSSTORAGE_HPP