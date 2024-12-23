#include "BookstoreOperation.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

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
    std::getline(std::cin, line);
    if (line.empty())
      continue;
    std::istringstream iss(line);
    iss >> op;
    if (op == "exit" || op == "quit") {
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
        bookstore.Login(userID);
      } else if (inputs.size() == 2) {
        userID = inputs[0];
        passWord = inputs[1];
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
      bookstore.UserAdd(userID, passWord, privilege, userName);
    } else if (op == "register") {
      std::string userID, passWord, userName;
      if (!(iss >> userID >> passWord >> userName) || (iss >> op)) {
        printError("Invalid parameters for register command");
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
        bookstore.Passwd(userID, newPassWord);
      } else if (inputs.size() == 3) {
        userID = inputs[0];
        passWord = inputs[1];
        newPassWord = inputs[2];
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
      if (iss >> op) {
        printError("Invalid parameters for select command");
        continue;
      }
      bookstore.Select(ISBN);
    }     else if (op == "modify") {
        std::string ISBN, BookName, Author, Keywords;
        float Price = -1;
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
                if (ISBN.empty()) {
                    printError("ISBN cannot be empty");
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
                if (BookName.empty()) {
                    printError("Book name cannot be empty");
                    hasError = true;
                    break;
                }
                if (BookName.front() == '"' && BookName.back() == '"') {
                    BookName = BookName.substr(1, BookName.length() - 2);
                }
            } else if (type.substr(0, 8) == "-author=") {
                if (!Author.empty()) {
                    printError("Duplicate author parameter");
                    hasError = true;
                    break;
                }
                Author = type.substr(8);
                if (Author.empty()) {
                    printError("Author cannot be empty");
                    hasError = true;
                    break;
                }
                if (Author.front() == '"' && Author.back() == '"') {
                    Author = Author.substr(1, Author.length() - 2);
                }
            } else if (type.substr(0, 9) == "-keyword=") {
                if (!Keywords.empty()) {
                    printError("Duplicate keyword parameter");
                    hasError = true;
                    break;
                }
                Keywords = type.substr(9);
                if (Keywords.empty()) {
                    printError("Keyword cannot be empty");
                    hasError = true;
                    break;
                }
                if (Keywords.front() == '"' && Keywords.back() == '"') {
                    Keywords = Keywords.substr(1, Keywords.length() - 2);
                }
                if (Keywords.find("||") != std::string::npos ||
                    Keywords.front() == '|' || Keywords.back() == '|') {
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
                if (PriceStr.empty()) {
                    printError("Price cannot be empty");
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
        if (ISBN.empty()) {
          printError("ISBN cannot be empty");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        bookstore.Show(ISBN);
      } else if (type.substr(0, 6) == "-name=") {
        std::string BookName = type.substr(6);
        if (BookName.empty()) {
          printError("Book name cannot be empty");
          continue;
        }

        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }

        if (BookName.front() == '"' && BookName.back() == '"') {
          BookName = BookName.substr(1, BookName.length() - 2);
        }
        bookstore.Show("", BookName);
      } else if (type.substr(0, 8) == "-author=") {
        std::string Author = type.substr(8);
        if (Author.empty()) {
          printError("Author cannot be empty");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        if (Author.front() == '"' && Author.back() == '"') {
          Author = Author.substr(1, Author.length() - 2);
        }
        bookstore.Show("", "", Author);
      } else if (type.substr(0, 9) == "-keyword=") {
        std::string Keyword = type.substr(9);
        if (Keyword.empty()) {
          printError("Keyword cannot be empty");
          continue;
        }
        if (iss >> op) {
          printError("Extra parameters for show command");
          continue;
        }
        if (Keyword.find("||") != std::string::npos || Keyword.front() == '|' ||
            Keyword.back() == '|') {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        if (Keyword.front() == '"' && Keyword.back() == '"') {
          Keyword = Keyword.substr(1, Keyword.length() - 2);
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
      if (!(iss >> quantity)) {
        printError("Missing quantity parameter");
        continue;
      }
      if (iss >> op) {
        printError("Extra parameters for buy command");
        continue;
      }
      bookstore.Buy(ISBN, quantity);
    } else if (op == "import") {
      float quantity, costPrice;
      if (!(iss >> quantity)) {
        printError("Missing quantity parameter");
        continue;
      }
      if (!(iss >> costPrice)) {
        printError("Missing cost price parameter");
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