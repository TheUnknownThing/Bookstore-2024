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

  bool validateQuantity(int quantity) {
    if (quantity <= 0) {
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

  void Login(const std::string &userID, const std::string &passWord = "") {
    // userStorage.PrintAll(); // debug
    if (!userStorage.Login(userID, passWord)) {
      std::cout << "Invalid" << std::endl;
      return;
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
      return;
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
      printError("Insufficient privileges to show books");
      return;
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
      printError("Insufficient privileges to make purchases");
      return;
    }
    if (!validateISBN(ISBN) || !validateQuantity(quantity)) {
      printError("Invalid ISBN or quantity value");
      return;
    }
    auto [flag, price] = bookStorage.buy(ISBN, quantity);
    if (!flag) {
      printError("Book not found or insufficient stock");
    } else {
      financeStorage.AddIncome(price);
    }
  }

  void Select(const std::string &ISBN) {
    if (!canExecute(SELECT)) {
      printError("Insufficient privileges to select books");
      return;
    }
    if (!bookStorage.select(ISBN)) {
      bookStorage.addBook(ISBN);
      int id = bookStorage.findIDByISBN(ISBN);
      userStorage.setCurrentUserSelection(id);
    } else {
      int id = bookStorage.findIDByISBN(ISBN);
      userStorage.setCurrentUserSelection(id);
    }
  }

  void Modify(const std::string &ISBN = "", const std::string &BookName = "",
              const std::string &Author = "", const std::string &Keyword = "",
              double Price = -1) {
    if (!canExecute(MODIFY)) {
      printError("Insufficient privileges to modify books");
      return;
    }

    int id = userStorage.getCurrentUserSelection();
    if (id == -1) {
      printError("No book selected for modification");
      return;
    }
    std::string currentISBN = bookStorage.getISBNByID(id);
    if (currentISBN == "") {
      printError("No book selected for modification");
      return;
    }

    // Validate all parameters first
    if (ISBN != "") {
      if (!validateISBN(ISBN)) {
        printError("Invalid ISBN format");
        return;
      }
      // Check if the new ISBN is already taken by another book
      if (bookStorage.isISBNExists(ISBN) && ISBN != currentISBN) {
        printError("ISBN already exists");
        return;
      }
    }

    if (ISBN == currentISBN) {
      printError("New ISBN cannot be the same as the current ISBN");
      return;
    }

    if (BookName != "" && !validateKeyword(BookName)) {
      printError("Invalid book name format");
      return;
    }

    if (Author != "" && !validateKeyword(Author)) {
      printError("Invalid author name format");
      return;
    }

    if (Keyword != "") {
      if (!validateKeyword(Keyword)) {
        printError("Invalid keyword format");
        return;
      }
      // Check for duplicate keywords
      std::set<std::string> uniqueKeywords;
      std::istringstream ss(Keyword);
      std::string kw;
      while (std::getline(ss, kw, '|')) {
        if (!uniqueKeywords.insert(kw).second) {
          printError("Duplicate keywords found");
          return;
        }
      }
    }

    if (Price != -1 && !validatePrice(Price)) {
      printError("Invalid price value");
      return;
    }

    // If all validations pass, apply the changes
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
  }

  void Import(double quantity, double costPrice) {
    if (!canExecute(IMPORT)) {
      printError("Insufficient privileges to import books");
      return;
    }
    if (!validateQuantity(quantity) || !validatePrice(costPrice)) {
      printError("Invalid quantity or cost price values");
      return;
    }
    int id = userStorage.getCurrentUserSelection();
    if (id == -1) {
      printError("No book selected for import");
      return;
    }
    std::string currentISBN = bookStorage.getISBNByID(id);
    if (currentISBN == "") {
      printError("No book selected for import");
      return;
    }
    bookStorage.import(currentISBN, quantity);
    financeStorage.AddOutcome(costPrice);
  }
  /*
   * @brief: Finance Operation
   * @functions: ShowFinance, ReportFinance
   */

  void ShowFinance(int count = -1) {
    if (!canExecute(SHOWFINANCE)) {
      std::cout << "Invalid" << std::endl;
      return;
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