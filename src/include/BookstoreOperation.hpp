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

  struct Log {
    OperationType op;
    char userID[31];
    union {
      FinancialLog financialLog;
      BookLog bookLog;
      UserLog userLog;
    };
  };

  User userStorage;
  Book bookStorage;
  Finance financeStorage;
  Logs logs;

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
    if (!userStorage.Login(userID, passWord)) {
      printError("Login failed");
      return false;
    }
    logs.AddLoginLog(userID);
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
    logs.AddUserLog(Logs::USERADD, userStorage.GetCurrentUserID(), userID, userName, privilege);
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
    logs.AddUserLog(Logs::REGISTER, userID, userID, userName, 1);
    return true;
  }

  bool Logout() {
    if (!canExecute(LOGOUT)) {
      printError("Insufficient privileges to logout");
      return false;
    }
    std::string currentUser = userStorage.GetCurrentUserID();
    logs.AddLogoutLog(currentUser);
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
    auto [flag, price] = bookStorage.buy(ISBN, quantity);
    if (!flag) {
      printError("Book not found or insufficient stock");
      return false;
    }
    financeStorage.AddIncome(price);
    logs.AddBookLog(Logs::BUY, userStorage.GetCurrentUserID(), ISBN, quantity, price);
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
      if (bookStorage.isISBNExists(ISBN) && ISBN != currentISBN) {
        printError("ISBN already exists");
        return false;
      }
    }

    if (ISBN == currentISBN) {
      printError("New ISBN cannot be the same as the current ISBN");
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
    logs.AddModifyLog(userStorage.GetCurrentUserID(), currentISBN);
    return true;
  }

  bool Import(double quantity, double costPrice) {
    if (!canExecute(IMPORT)) {
      printError("Insufficient privileges to import books");
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
    logs.AddBookLog(Logs::IMPORT, userStorage.GetCurrentUserID(), currentISBN, quantity, costPrice);
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
      return;
    }
    
    std::cout << "\n=== Financial Report ===\n" << std::endl;
    
    double totalIncome = financeStorage.GetTotalIncome();
    double totalOutcome = financeStorage.GetTotalOutcome();
    double netProfit = totalIncome - totalOutcome;
    
    std::cout << "Overall Financial Summary:" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Income:  + " << totalIncome << std::endl;
    std::cout << "Total Expense: - " << totalOutcome << std::endl;
    std::cout << "Net Profit:    " << (netProfit >= 0 ? "+" : "") << netProfit << std::endl;

    std::cout << "\n=== End of Financial Report ===\n" << std::endl;
  }

  /*
   * @brief: Employee Operation
   * @functions: ReportEmployee
   */

  void ReportEmployee() {
    if (!canExecute(REPORTEMPLOYEE)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    
    logs.ShowEmployeeLog();
    
  }

  /*
   * @brief: Log Operation
   * @functions: Log
   */

  void Log() {
    if (!canExecute(LOG)) {
      std::cout << "Invalid" << std::endl;
      return;
    }
    
    logs.ShowLog();
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