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
    PASSWDROOT,
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
      {LOGIN, 0},       {LOGOUT, 1},        {USERADD, 3},        {REGISTER, 0},
      {PASSWD, 1},      {PASSWDROOT, 7},    {DELETE, 7},         {SHOW, 1},
      {BUY, 1},         {SELECT, 3},        {MODIFY, 3},         {IMPORT, 3},
      {SHOWFINANCE, 7}, {REPORTFINANCE, 7}, {REPORTEMPLOYEE, 7}, {LOG, 7}};

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

  bool validateISBN(const std::string &ISBN) {
    if (ISBN.size() != 13) {
      return false;
    }
    for (int i = 0; i < 13; i++) {
      if (!isdigit(ISBN[i])) {
        return false;
      }
    }
    return true;
  }

  bool validateKeyword(const std::string &keyword) {
    if (keyword.size() > 60) {
      return false;
    }
    return true;
  }

  bool validateQuantity(int quantity) {
    if (quantity < 0) {
      return false;
    }
    return true;
  }

  bool validatePrice(float price) {
    if (price < 0) {
      return false;
    }
    return true;
  }

public:
  BookstoreOperation() {}

  bool canExecute(OperationType op) {
    return userStorage.GetCurrentUserPrivilege() >= opPrivilege[op];
  }

  /*
   * @brief: User Operation
   * @functions: Login, Logout, UserAdd, Register, Passwd, DeleteUser
   * @stat: basic testcase 1-3 passed
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
    if (!canExecute(PASSWDROOT)) {
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
  void Show(const std::string &ISBN = "", const std::string &BookName = "",
            const std::string &Author = "", const std::string &Keyword = "") {
    if (!canExecute(SHOW)) {
      std::cout << "Invalid" << std::endl;
    }
    if (ISBN != "") {
      bookStorage.show(ISBN);
    } else if (BookName != "") {
      bookStorage.show("", BookName);
    } else if (Author != "") {
      bookStorage.show("", "", Author);
    } else if (Keyword != "") {
      bookStorage.show("", "", "", Keyword);
    } else {
      bookStorage.show();
    }
  }

  void Buy(const std::string &ISBN, int quantity) {
    if (!canExecute(BUY)) {
      std::cout << "Invalid" << std::endl;
    }
    if (!bookStorage.buy(ISBN, quantity)) {
      std::cout << "Invalid" << std::endl;
    }
    // logStorage.LogAdd(BUY, userStorage.GetUserID());
  }

  void Select(const std::string &ISBN) {
    if (!canExecute(SELECT)) {
      std::cout << "Invalid" << std::endl;
    }
    if (!bookStorage.select(ISBN)) {
      // add a new book with ISBN
      bookStorage.addBook(ISBN);
      userStorage.setCurrentUserSelection(ISBN);
    } else {
      userStorage.setCurrentUserSelection(ISBN);
    }
    // logStorage.LogAdd(SELECT, userStorage.GetUserID());
  }

  void Modify(const std::string &ISBN = "", const std::string &BookName = "",
              const std::string &Author = "", const std::string &Keyword = "",
              float Price = 0) {
    if (!canExecute(MODIFY)) {
      std::cout << "Invalid" << std::endl;
    }
    std::string currentISBN = userStorage.getCurrentUserSelection();
    if (currentISBN == "") {
      std::cout << "Invalid" << std::endl;
    } else {
      if (ISBN != "") {
        // validate ISBN
        if (!validateISBN(ISBN)) {
          std::cout << "Invalid" << std::endl;
          return;
        }
        bookStorage.modify(currentISBN, ISBN);
      } else if (BookName != "") {
        if (!validateKeyword(BookName)) {
          std::cout << "Invalid" << std::endl;
          return;
        }
        bookStorage.modify(currentISBN, "", BookName);
      } else if (Author != "") {
        if (!validateKeyword(Author)) {
          std::cout << "Invalid" << std::endl;
          return;
        }
        bookStorage.modify(currentISBN, "", "", Author);
      } else if (Keyword != "") {
        if (!validateKeyword(Keyword)) {
          std::cout << "Invalid" << std::endl;
          return;
        }
        bookStorage.modify(currentISBN, "", "", "", Keyword);
      } else if (Price != 0) {
        if (!validatePrice(Price)) {
          std::cout << "Invalid" << std::endl;
          return;
        }
        bookStorage.modify(currentISBN, "", "", "", "", Price);
      } else {
        std::cout << "Invalid" << std::endl;
      }
    }
    // logStorage.LogAdd(MODIFY, userStorage.GetUserID());
  }

  void Import(float quantity, float costPrice) {
    if (!canExecute(IMPORT)) {
      std::cout << "Invalid" << std::endl;
    }
    if (!validateQuantity(quantity) || !validatePrice(costPrice)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    std::string currentISBN = userStorage.getCurrentUserSelection();
    if (currentISBN == "") {
      std::cout << "Invalid" << std::endl;
      return;
    }
    bookStorage.import(currentISBN, quantity, costPrice);
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
    
  }
};

#endif // BOOKSTOREOPERATION_HPP