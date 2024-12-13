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

  struct Log {
    OperationType op;
    char userID[31];
    
  };

  FileOperation<Log> logFile;

public:
  Logs() { logFile.initialise("log.dat"); }

  void LogAdd() {
    
  }

  void ShowLog() {
    
  }
};

#endif // LOGSSTORAGE_HPP