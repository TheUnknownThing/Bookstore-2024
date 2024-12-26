#include "BookstoreOperation.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

bool isValidUserIDOrPassword(const std::string &str) {
  if (str.length() > 30)
    return false;
  return std::all_of(str.begin(), str.end(),
                     [](char c) { return std::isalnum(c) || c == '_'; });
}

bool isValidUsername(const std::string &str) {
  if (str.length() > 30)
    return false;
  return std::all_of(str.begin(), str.end(), [](char c) {
    return c >= 32 && c <= 126;
  });
}

bool isValidISBN(const std::string &str) {
  if (str.length() > 20)
    return false;
  return std::all_of(str.begin(), str.end(), [](char c) {
    return c >= 32 && c <= 126;
  });
}

bool isValidBookNameOrAuthor(const std::string &str) {
  if (str.length() > 60)
    return false;
  return std::all_of(str.begin(), str.end(), [](char c) {
    return (c >= 32 && c <= 126 &&
            c != '"');
  });
}

bool isValidKeywords(const std::string &str) {
  if (str.length() > 60)
    return false;
  if (str.empty() || str.front() == '|' || str.back() == '|')
    return false;

  std::string prev;
  std::istringstream ss(str);
  std::string token;
  std::set<std::string> keywords;

  while (std::getline(ss, token, '|')) {
    if (token.empty())
      return false;
    if (token.find('"') != std::string::npos)
      return false;
    if (!keywords.insert(token).second)
      return false; // Duplicate keyword
  }
  return true;
}

bool isValidQuantity(const std::string &str) {
  if (str.empty() || str.length() > 13)
    return false;
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool isValidPrice(const std::string &str) {
  if (str.empty() || str.length() > 13)
    return false;

  if (str.front() == '-')
    return false;

  size_t dotPos = str.find('.');
  if (dotPos == std::string::npos) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
  }

  return std::all_of(str.begin(), str.begin() + dotPos, ::isdigit) &&
         std::all_of(str.begin() + dotPos + 1, str.end(), ::isdigit);
}

void printError(const std::string &detailedMessage) {
#ifdef DEBUG_MODE
  std::cout << "Error: " << detailedMessage << std::endl;
#else
  std::cout << "Invalid" << std::endl;
#endif
}

int main() {
  std::string line, op;
  BookstoreOperation bookstore;
  while (true) {
    if (std::cin.eof()) break;

    std::getline(std::cin, line);
    if (line.empty()) continue;
    std::istringstream iss(line);
    iss >> op;

    if (op.empty()) {
      continue;
    } else if (op == "exit" || op == "quit") {
      if (!(iss >> std::ws).eof()) {
        printError("Extra parameters for exit/quit command");
        continue;
      }
      break;
    } else if (op == "su") {
      std::string userID, passWord;
      std::vector<std::string> inputs;
      std::string temp;
      while (iss >> temp) {
        inputs.push_back(temp);
      }

      if (inputs.size() == 1) {
        userID = inputs[0];
        if (!isValidUserIDOrPassword(userID)) {
          printError("Invalid UserID format");
          continue;
        }
        bookstore.Login(userID);
      } else if (inputs.size() == 2) {
        userID = inputs[0];
        passWord = inputs[1];
        if (!isValidUserIDOrPassword(userID) ||
            !isValidUserIDOrPassword(passWord)) {
          printError("Invalid UserID or Password format");
          continue;
        }
        bookstore.Login(userID, passWord);
      } else {
        printError("Invalid number of parameters for login");
      }
    } else if (op == "logout") {
      if (!(iss >> std::ws).eof()) {
        printError("Extra parameters for logout command");
        continue;
      }
      bookstore.Logout();
    } else if (op == "useradd") {
      std::string userID, passWord, userName;
      int privilege;
      if (!(iss >> userID >> passWord >> privilege >> userName) ||
          (iss >> op)) {
        printError("Invalid parameters for useradd command");
        continue;
      }
      if (!isValidUserIDOrPassword(userID)) {
        printError("Invalid UserID format");
        continue;
      }
      if (!isValidUserIDOrPassword(passWord)) {
        printError("Invalid Password format");
        continue;
      }
      if (!isValidUsername(userName)) {
        printError("Invalid Username format");
        continue;
      }
      if (privilege != 1 && privilege != 3 && privilege != 7) {
        printError("Invalid privilege level");
        continue;
      }
      bookstore.UserAdd(userID, passWord, privilege, userName);
    } else if (op == "register") {
      std::string userID, passWord, userName;
      if (!(iss >> userID >> passWord >> userName) || (iss >> op)) {
        printError("Invalid parameters for register command");
        continue;
      }
      if (!isValidUserIDOrPassword(userID)) {
        printError("Invalid UserID format");
        continue;
      }
      if (!isValidUserIDOrPassword(passWord)) {
        printError("Invalid Password format");
        continue;
      }
      if (!isValidUsername(userName)) {
        printError("Invalid Username format");
        continue;
      }
      bookstore.Register(userID, passWord, userName);
    } else if (op == "passwd") {
      std::string userID, passWord, newPassWord;
      std::vector<std::string> inputs;
      std::string temp;
      while (iss >> temp) {
        inputs.push_back(temp);
      }

      if (inputs.size() == 2) {
        userID = inputs[0];
        newPassWord = inputs[1];
        if (!isValidUserIDOrPassword(userID) ||
            !isValidUserIDOrPassword(newPassWord)) {
          printError("Invalid UserID or Password format");
          continue;
        }
        bookstore.Passwd(userID, newPassWord);
      } else if (inputs.size() == 3) {
        userID = inputs[0];
        passWord = inputs[1];
        newPassWord = inputs[2];
        if (!isValidUserIDOrPassword(userID) ||
            !isValidUserIDOrPassword(passWord) ||
            !isValidUserIDOrPassword(newPassWord)) {
          printError("Invalid UserID or Password format");
          continue;
        }
        bookstore.Passwd(userID, passWord, newPassWord);
      } else {
        printError("Invalid number of parameters for passwd");
      }
    } else if (op == "delete") {
      std::string userID;
      if (!(iss >> userID) || (iss >> op)) {
        printError("Invalid parameters for delete command");
        continue;
      }
      bookstore.DeleteUser(userID);
    } else if (op == "select") {
      std::string ISBN;
      if (!(iss >> ISBN)) {
        printError("Invalid parameters for select command");
        continue;
      }
      if (!isValidISBN(ISBN)) {
        printError("Invalid ISBN format");
        continue;
      }
      if (iss >> op) {
        printError("Invalid parameters for select command");
        continue;
      }
      bookstore.Select(ISBN);
    } else if (op == "modify") {
      std::string ISBN, BookName, Author, Keywords;
      double Price = -1;
      std::string type;
      bool hasError = false;

      while (iss >> type && !hasError) {
        if (type.substr(0, 6) == "-ISBN=") {
          if (!ISBN.empty()) {
            printError("Duplicate ISBN parameter");
            hasError = true;
            break;
          }
          ISBN = type.substr(6);
          if (!isValidISBN(ISBN)) {
            printError("Invalid ISBN format");
            hasError = true;
            break;
          }
        } else if (type.substr(0, 6) == "-name=") {
          if (!BookName.empty()) {
            printError("Duplicate name parameter");
            hasError = true;
            break;
          }
          BookName = type.substr(6);
          if (BookName.length() < 2 || BookName.front() != '"' ||
              BookName.back() != '"') {
            printError("Book name must be enclosed in quotes");
            hasError = true;
            break;
          }
          BookName = BookName.substr(1, BookName.length() - 2);
          if (!isValidBookNameOrAuthor(BookName)) {
            printError("Invalid book name format");
            hasError = true;
            break;
          }
        } else if (type.substr(0, 8) == "-author=") {
          if (!Author.empty()) {
            printError("Duplicate author parameter");
            hasError = true;
            break;
          }
          Author = type.substr(8);
          if (Author.length() < 2 || Author.front() != '"' ||
              Author.back() != '"') {
            printError("Author must be enclosed in quotes");
            hasError = true;
            break;
          }
          Author = Author.substr(1, Author.length() - 2);
          if (!isValidBookNameOrAuthor(Author)) {
            printError("Invalid author format");
            hasError = true;
            break;
          }
        } else if (type.substr(0, 9) == "-keyword=") {
          if (!Keywords.empty()) {
            printError("Duplicate keyword parameter");
            hasError = true;
            break;
          }
          Keywords = type.substr(9);
          if (Keywords.length() < 2 || Keywords.front() != '"' ||
              Keywords.back() != '"') {
            printError("Keywords must be enclosed in quotes");
            hasError = true;
            break;
          }
          Keywords = Keywords.substr(1, Keywords.length() - 2);
          if (!isValidKeywords(Keywords)) {
            printError("Invalid keyword format");
            hasError = true;
            break;
          }
        } else if (type.substr(0, 7) == "-price=") {
          if (Price != -1) {
            printError("Duplicate price parameter");
            hasError = true;
            break;
          }
          std::string PriceStr = type.substr(7);
          if (!isValidPrice(PriceStr)) {
            printError("Invalid price format");
            hasError = true;
            break;
          }
          try {
            Price = std::stof(PriceStr);
            if (Price < 0) {
              printError("Price cannot be negative");
              hasError = true;
              break;
            }
          } catch (const std::exception &) {
            printError("Invalid price format");
            hasError = true;
            break;
          }
        } else {
          printError("Invalid parameters for modify command");
          hasError = true;
          break;
        }
      }

      if (!hasError) {
        bookstore.Modify(ISBN, BookName, Author, Keywords, Price);
      }
    } else if (op == "show") {
      std::string type;
      if (!(iss >> type)) {
        bookstore.Show();
        continue;
      }
      if (type.substr(0, 6) == "-ISBN=") {
        std::string ISBN = type.substr(6);
        if (!isValidISBN(ISBN)) {
          printError("Invalid ISBN format");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        bookstore.Show(ISBN);
      } else if (type.substr(0, 6) == "-name=") {
        std::string BookName = type.substr(6);
        if (BookName.length() < 2 || BookName.front() != '"' ||
            BookName.back() != '"') {
          printError("Book name must be enclosed in quotes");
          continue;
        }
        BookName = BookName.substr(1, BookName.length() - 2);
        if (!isValidBookNameOrAuthor(BookName)) {
          printError("Invalid book name format");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        bookstore.Show("", BookName);
      } else if (type.substr(0, 8) == "-author=") {
        std::string Author = type.substr(8);
        if (Author.length() < 2 || Author.front() != '"' ||
            Author.back() != '"') {
          printError("Author must be enclosed in quotes");
          continue;
        }
        Author = Author.substr(1, Author.length() - 2);
        if (!isValidBookNameOrAuthor(Author)) {
          printError("Invalid author format");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        bookstore.Show("", "", Author);
      } else if (type.substr(0, 9) == "-keyword=") {
        std::string Keyword = type.substr(9);
        if (Keyword.length() < 2 || Keyword.front() != '"' ||
            Keyword.back() != '"') {
          printError("Keyword must be enclosed in quotes");
          continue;
        }
        Keyword = Keyword.substr(1, Keyword.length() - 2);
        if (Keyword.find("|") != std::string::npos) {
          printError("Keywords must be enclosed in quotes");
          continue;
        }
        if (!isValidKeywords(Keyword)) {
          printError("Invalid keyword format");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        bookstore.Show("", "", "", Keyword);
      } else if (type == "finance") {
        int count = -1;
        if (iss >> count) {
          if (count < 0) {
            printError("Invalid count parameter");
            continue;
          } else if (iss >> op) {
            printError("Extra parameters for show finance command");
            continue;
          }
          bookstore.ShowFinance(count);
        } else {
          if (iss >> op) {
            printError("Extra parameters for show finance command");
            continue;
          }
          bookstore.ShowFinance();
        }
      } else {
        printError("Invalid parameters for show command");
      }
    } else if (op == "buy") {
      std::string ISBN;
      int quantity;
      if (!(iss >> ISBN)) {
        printError("Missing ISBN parameter");
        continue;
      }
      if (!isValidISBN(ISBN)) {
        printError("Invalid ISBN format");
        continue;
      }
      if (!(iss >> quantity)) {
        printError("Missing quantity parameter");
        continue;
      }
      if (quantity <= 0 || quantity > 2147483647) {
        printError("Invalid quantity");
        continue;
      }
      if (iss >> op) {
        printError("Extra parameters for buy command");
        continue;
      }
      bookstore.Buy(ISBN, quantity);
    } else if (op == "import") {
      double quantity, costPrice;
      std::string quantityStr, costPriceStr;
      if (!(iss >> quantityStr)) {
        printError("Missing quantity parameter");
        continue;
      }
      if (!(iss >> costPriceStr)) {
        printError("Missing cost price parameter");
        continue;
      }
      if (!isValidPrice(costPriceStr)) {
        printError("Invalid cost price format");
        continue;
      }
      if (!isValidQuantity(quantityStr)) {
        printError("Invalid quantity format");
        continue;
      }
      try {
        quantity = std::stof(quantityStr);
        costPrice = std::stof(costPriceStr);
        if (quantity <= 0 || quantity > 2147483647) {
          printError("Invalid quantity");
          continue;
        }
        if (costPrice <= 0) {
          printError("Cost price must be positive");
          continue;
        }
      } catch (const std::exception &) {
        printError("Invalid number format");
        continue;
      }
      if (iss >> op) {
        printError("Extra parameters for import command");
        continue;
      }
      bookstore.Import(quantity, costPrice);
    } else {
      printError("Unknown command");
    }
  }
  return 0;
}