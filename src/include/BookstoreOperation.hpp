#ifndef BOOKSTOREOPERATION_HPP
#define BOOKSTOREOPERATION_HPP

#include "BookStorage.hpp"
#include "FinanceStorage.hpp"
#include "LogsStorage.hpp"
#include "UserStorage.hpp"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
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
  Finance financeStorage;

  bool validateISBN(const std::string &ISBN) {
    return (ISBN.size() <= 20 && ISBN.size() > 0);
  }

  bool validateKeyword(const std::string &keyword) {
    if (keyword.size() > 60) {
      return false;
    }
    return true;
  }

  bool validateQuantity(double quantity) {
    if (quantity <= 0) {
      return false;
    }
    if (quantity - static_cast<int>(quantity) > 0) {
      return false;
    }
    return true;
  }

  bool validatePrice(double price) {
    if (price < 0) {
      return false;
    }
    return true;
  }

  void printError(const std::string &detailedMessage) {
#ifdef DEBUG_MODE
    std::cout << "Error: " << detailedMessage << std::endl;
#else
    std::cout << "Invalid" << std::endl;
#endif
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

  bool Login(const std::string &userID, const std::string &passWord = "") {
    // userStorage.PrintAll(); // debug
    if (!userStorage.Login(userID, passWord)) {
      printError("Login failed");
      return false;
    }
    return true;
  }

  bool UserAdd(const std::string &userID, const std::string &passWord,
               int privilege, const std::string &userName) {
    if (!canExecute(USERADD)) {
      printError("Insufficient privileges to add users");
      return false;
    }
    if (!userStorage.UserAdd(userID, passWord, userName, privilege)) {
      printError("Add user failed");
      return false;
    }
    return true;
  }

  bool Passwd(const std::string &userID, const std::string &newPassWord) {
    if (!canExecute(PASSWDROOT)) {
      printError("Insufficient privileges to change password");
      return false;
    }
    if (!userStorage.Passwd(userID, newPassWord)) {
      printError("Change password failed");
      return false;
    }
    return true;
  }

  bool Passwd(const std::string &userID, const std::string &passWord,
              const std::string &newPassWord) {
    if (!canExecute(PASSWD)) {
      printError("Insufficient privileges to change password");
      return false;
    }
    if (!userStorage.Passwd(userID, passWord, newPassWord)) {
      printError("Change password failed");
      return false;
    }
    return true;
  }

  bool DeleteUser(const std::string &userID) {
    if (!canExecute(DELETE)) {
      printError("Insufficient privileges to delete users");
      return false;
    }
    if (!userStorage.DeleteUser(userID)) {
      printError("Delete user failed");
      return false;
    }
    return true;
  }

  bool Register(const std::string &userID, const std::string &passWord,
                const std::string &userName) {
    if (!userStorage.Register(userID, passWord, userName)) {
      printError("Register failed");
      return false;
    }
    return true;
  }

  bool Logout() {
    if (!canExecute(LOGOUT)) {
      printError("Insufficient privileges to logout");
      return false;
    }
    userStorage.Logout();
    return true;
  }

  /*
   * @brief: Book Operation
   * @functions: Show, Buy, Select, Modify, Import
   */
  bool Show(const std::string &ISBN = "", const std::string &BookName = "",
            const std::string &Author = "", const std::string &Keyword = "") {
    if (!canExecute(SHOW)) {
      printError("Insufficient privileges to show books");
      return false;
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
    return true;
  }

  bool Buy(const std::string &ISBN, int quantity) {
    if (!canExecute(BUY)) {
      printError("Insufficient privileges to make purchases");
      return false;
    }
    if (!validateISBN(ISBN) || !validateQuantity(quantity)) {
      printError("Invalid ISBN or quantity value");
      return false;
    }
    auto [flag, price] = bookStorage.buy(ISBN, quantity);
    if (!flag) {
      printError("Book not found or insufficient stock");
    } else {
      financeStorage.AddIncome(price);
    }
    return true;
  }

  bool Select(const std::string &ISBN) {
    if (!canExecute(SELECT)) {
      printError("Insufficient privileges to select books");
      return false;
    }
    if (!bookStorage.select(ISBN)) {
      bookStorage.addBook(ISBN);
      int id = bookStorage.findIDByISBN(ISBN);
      userStorage.setCurrentUserSelection(id);
    } else {
      int id = bookStorage.findIDByISBN(ISBN);
      userStorage.setCurrentUserSelection(id);
    }
    return true;
  }

  bool Modify(const std::string &ISBN = "", const std::string &BookName = "",
              const std::string &Author = "", const std::string &Keyword = "",
              double Price = -1) {
    if (!canExecute(MODIFY)) {
      printError("Insufficient privileges to modify books");
      return false;
    }

    int id = userStorage.getCurrentUserSelection();
    if (id == -1) {
      printError("No book selected for modification");
      return false;
    }
    std::string currentISBN = bookStorage.getISBNByID(id);
    if (currentISBN == "") {
      printError("No book selected for modification");
      return false;
    }

    if (ISBN != "") {
      if (!validateISBN(ISBN)) {
        printError("Invalid ISBN format");
        return false;
      }
      if (bookStorage.isISBNExists(ISBN) && ISBN != currentISBN) {
        printError("ISBN already exists");
        return false;
      }
    }

    if (ISBN == currentISBN) {
      printError("New ISBN cannot be the same as the current ISBN");
      return false;
    }

    if (BookName != "" && !validateKeyword(BookName)) {
      printError("Invalid book name format");
      return false;
    }

    if (Author != "" && !validateKeyword(Author)) {
      printError("Invalid author name format");
      return false;
    }

    if (Keyword != "") {
      if (!validateKeyword(Keyword)) {
        printError("Invalid keyword format");
        return false;
      }
      // Check for duplicate keywords
      std::set<std::string> uniqueKeywords;
      std::istringstream ss(Keyword);
      std::string kw;
      while (std::getline(ss, kw, '|')) {
        if (!uniqueKeywords.insert(kw).second) {
          printError("Duplicate keywords found");
          return false;
        }
      }
    }

    if (Price != -1 && !validatePrice(Price)) {
      printError("Invalid price value");
      return false;
    }

    if (ISBN != "") {
      bookStorage.modify(currentISBN, ISBN);
    }
    if (BookName != "") {
      bookStorage.modify(currentISBN, "", BookName);
    }
    if (Author != "") {
      bookStorage.modify(currentISBN, "", "", Author);
    }
    if (Keyword != "") {
      bookStorage.modify(currentISBN, "", "", "", Keyword);
    }
    if (Price != -1) {
      bookStorage.modify(currentISBN, "", "", "", "", Price);
    }
    return true;
  }

  bool Import(double quantity, double costPrice) {
    if (!canExecute(IMPORT)) {
      printError("Insufficient privileges to import books");
      return false;
    }
    if (!validateQuantity(quantity) || !validatePrice(costPrice)) {
      printError("Invalid quantity or cost price values");
      return false;
    }
    int id = userStorage.getCurrentUserSelection();
    if (id == -1) {
      printError("No book selected for import");
      return false;
    }
    std::string currentISBN = bookStorage.getISBNByID(id);
    if (currentISBN == "") {
      printError("No book selected for import");
      return false;
    }
    bookStorage.import(currentISBN, quantity);
    financeStorage.AddOutcome(costPrice);
    return true;
  }
  /*
   * @brief: Finance Operation
   * @functions: ShowFinance, ReportFinance
   */

  bool ShowFinance(int count = -1) {
    if (!canExecute(SHOWFINANCE)) {
      std::cout << "Invalid" << std::endl;
      return false;
    }
    if (count == -1) {
      // show all
      double totalIncome = 0, totalOutcome = 0;
      totalIncome = financeStorage.GetTotalIncome();
      totalOutcome = financeStorage.GetTotalOutcome();
      std::cout << std::setprecision(2) << std::fixed << "+ " << totalIncome
                << " - " << totalOutcome << std::endl;
    } else if (count == 0) {
      std::cout << std::endl;
    } else {
      // show count records
      auto [income, outcome] = financeStorage.GetLastNSum(count);
      if (income == -1 && outcome == -1) {
        std::cout << "Invalid" << std::endl;
      } else {
        std::cout << std::setprecision(2) << std::fixed << "+ " << income
                  << " - " << outcome << std::endl;
      }
    }
    return true;
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

  /*
   * @brief: Debug Operation
   * @functions: Debug
   */
  void DebugUser() {
    userStorage.PrintAll();
    userStorage.ShowAllLogInUsers();
  }
};

#endif // BOOKSTOREOPERATION_HPP