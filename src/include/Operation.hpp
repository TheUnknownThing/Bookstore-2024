#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

class Operation {
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

  std::map<OperationType, int> opPrivilege = {
      {LOGIN, 0},          {LOGOUT, 0},          {USERADD, 7}, {REGISTER, 1},
      {PASSWD, 2},         {DELETE, 3},          {SHOW, 4},    {BUY, 5},
      {SELECT, 6},         {MODIFY, 7},          {IMPORT, 8},  {SHOWFINANCE, 9},
      {REPORTFINANCE, 10}, {REPORTEMPLOYEE, 11}, {LOG, 12}};
  
  std::map<std::string, OperationType> opMap = {
      {"login", LOGIN},
      {"logout", LOGOUT},
      {"useradd", USERADD},
      {"register", REGISTER},
      {"passwd", PASSWD},
      {"delete", DELETE},
      {"show", SHOW},
      {"buy", BUY},
      {"select", SELECT},
      {"modify", MODIFY},
      {"import", IMPORT},
      {"showfinance", SHOWFINANCE},
      {"reportfinance", REPORTFINANCE},
      {"reportemployee", REPORTEMPLOYEE},
      {"log", LOG}};

public:
};

#endif // OPERATION_HPP