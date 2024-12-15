#ifndef BOOKSTOREOPERATION_HPP
#define BOOKSTOREOPERATION_HPP

#include "BookStorage.hpp"
#include "FinanceStorage.hpp"
#include "LogsStorage.hpp"
#include "UserStorage.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

class BookstoreOperation {
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
      {LOGIN, 0},          {LOGOUT, 0},          {USERADD, 3}, {REGISTER, 0},
      {PASSWD, 1},         {DELETE, 7},          {SHOW, 4},    {BUY, 5},
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

  User userStorage;
  Book bookStorage;
  Logs logStorage;
  Finance financeStorage;

public:
  BookstoreOperation() {}

  bool canExecute(OperationType op) {
    return userStorage.GetCurrentUserPrivilege() >= opPrivilege[op];
  }

  /*
   * @brief: User Operation
   * @functions: Login, Logout, UserAdd, Register, Passwd, DeleteUser
   */

  void Login(const std::string &userID, const std::string &passWord = "") {
    if (!userStorage.Login(userID, passWord)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void UserAdd(const std::string &userID, const std::string &passWord,
               int privilege, const std::string &userName) {
    if (!canExecute(USERADD)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (!userStorage.UserAdd(userID, passWord, userName, privilege)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void Passwd(const std::string &userID, const std::string &newPassWord) {
    if (!canExecute(PASSWD)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (!userStorage.Passwd(userID, newPassWord)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void Passwd(const std::string &userID, const std::string &passWord,
              const std::string &newPassWord) {
    if (!canExecute(PASSWD)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (!userStorage.Passwd(userID, passWord, newPassWord)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void DeleteUser(const std::string &userID) {
    if (!canExecute(DELETE)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (!userStorage.DeleteUser(userID)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void Register(const std::string &userID, const std::string &passWord,
                const std::string &userName) {
    if (!canExecute(REGISTER)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (!userStorage.Register(userID, passWord, userName)) {
      std::cout << "Invalid" << std::endl;
    }
  }

  void Logout() {
    if (!canExecute(LOGOUT)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    userStorage.Logout();
  }

  /*
   * @brief: Book Operation
   * @functions: Show, Buy, Select, Modify, Import
   */

  void Show() {
    if (!canExecute(SHOW)) {
      std::cout << "Invalid" << std::endl;
    }
    // TODO
  }

  void Buy() {
    if (!canExecute(BUY)) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(BUY, userStorage.GetUserID());
  }

  void Select() {
    if (!canExecute(SELECT)) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(SELECT, userStorage.GetUserID());
  }

  void Modify() {
    if (!canExecute(MODIFY)) {
      std::cout << "Invalid" << std::endl;
    }

    // logStorage.LogAdd(MODIFY, userStorage.GetUserID());
  }

  void Import() {
    if (!canExecute(IMPORT)) {
      std::cout << "Invalid" << std::endl;
    }
    std::string ISBN;
    int quantity, costPrice;
    std::cin >> ISBN >> quantity >> costPrice;

    // logStorage.LogAdd(IMPORT, userStorage.GetUserID());
  }

  /*
   * @brief: Finance Operation
   * @functions: ShowFinance, ReportFinance
   */

  void ShowFinance() {
    if (!canExecute(SHOWFINANCE)) {
      std::cout << "Invalid" << std::endl;
    }
    // TODO
  }

  void ReportFinance() {
    if (!canExecute(REPORTFINANCE)) {
      std::cout << "Invalid" << std::endl;
    }
    // TODO
  }

  /*
   * @brief: Employee Operation
   * @functions: ReportEmployee
   */

  void ReportEmployee() {
    if (!canExecute(REPORTEMPLOYEE)) {
      std::cout << "Invalid" << std::endl;
    }
    // TODO
  }

  /*
   * @brief: Log Operation
   * @functions: Log
   */

  void Log() {
    if (!canExecute(LOG)) {
      std::cout << "Invalid" << std::endl;
    }
    logStorage.ShowLog();
  }
};

#endif // BOOKSTOREOPERATION_HPP