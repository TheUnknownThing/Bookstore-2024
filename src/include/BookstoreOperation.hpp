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

  void Login() {
    // userStorage.PrintAll(); // debug
    std::string userID, passWord;
    std::cin >> userID >> passWord;
    bool flag = userStorage.Login(userID, passWord);
    if (!flag) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(LOGIN, userID);
  }

  void Logout() {
    userStorage.Logout();
    // logStorage.LogAdd(LOGOUT, userStorage.GetUserID());
  }

  void UserAdd() {
    if (!canExecute(USERADD)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    std::string userID, passWord, userName;
    int privilege;
    std::cin >> userID >> passWord >> userName >> privilege;
    userStorage.UserAdd(userID, passWord, userName, privilege);
    // logStorage.LogAdd(USERADD, userStorage.GetUserID());
  }

  void Register() {
    std::string userID, passWord, userName;
    std::cin >> userID >> passWord >> userName;
    bool flag = userStorage.Register(userID, passWord, userName);
    if (!flag) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(REGISTER, userStorage.GetUserID());
  }

  void Passwd() {
    if (!canExecute(PASSWD)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    if (userStorage.GetCurrentUserPrivilege() == 7) {
      // root
      std::string userID, newPassWord;
      std::cin >> userID >> newPassWord;
      userStorage.Passwd(userID, newPassWord);
    } else {
      std::string userID, passWord, newPassWord;
      std::cin >> userID >> passWord >> newPassWord;
      userStorage.Passwd(userID, passWord, newPassWord);
    }
    // logStorage.LogAdd(PASSWD, userStorage.GetUserID());
  }

  void DeleteUser() {
    if (!canExecute(DELETE)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    std::string userID;
    std::cin >> userID;
    bool flag = userStorage.DeleteUser(userID);
    if (!flag) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(DELETE, userStorage.GetUserID());
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
    std::string ISBN;
    int quantity;
    std::cin >> ISBN >> quantity;
    bookStorage.Buy(ISBN, quantity);
    // logStorage.LogAdd(BUY, userStorage.GetUserID());
  }

  void Select() {
    if (!canExecute(SELECT)) {
      std::cout << "Invalid" << std::endl;
    }
    std::string ISBN;
    std::cin >> ISBN;
    bookStorage.Select(ISBN);
    // logStorage.LogAdd(SELECT, userStorage.GetUserID());
  }

  void Modify() {
    if (!canExecute(MODIFY)) {
      std::cout << "Invalid" << std::endl;
    }
    std::string ISBN;
    int quantity, costPrice, sellingPrice;
    std::cin >> ISBN >> quantity >> costPrice >> sellingPrice;

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