#include "BookstoreOperation.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

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
        std::cout << "Invalid" << std::endl;
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
        std::cout << "Invalid" << std::endl;
      }
    } else if (op == "logout") {
      if (!(iss >> std::ws).eof()) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Logout();
    } else if (op == "useradd") {
      std::string userID, passWord, userName;
      int privilege;
      if (!(iss >> userID >> passWord >> privilege >> userName) ||
          (iss >> op)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.UserAdd(userID, passWord, privilege, userName);
    } else if (op == "register") {
      std::string userID, passWord, userName;
      if (!(iss >> userID >> passWord >> userName) || (iss >> op)) {
        std::cout << "Invalid" << std::endl;
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
        std::cout << "Invalid" << std::endl;
      }
    } else if (op == "delete") {
      std::string userID;
      if (!(iss >> userID) || (iss >> op)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.DeleteUser(userID);
    } else if (op == "select") {
      std::string ISBN;
      if (!(iss >> ISBN)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (iss >> op) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Select(ISBN);
    } else if (op == "modify") {
      std::string ISBN, BookName, Author, Keywords;
      float Price = -1;
      std::string type;

      while (iss >> type) {
        if (type.substr(0, 6) == "-ISBN=") {
          ISBN = type.substr(6);
          if (ISBN.empty()) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          bookstore.Modify(ISBN);
        } else if (type.substr(0, 6) == "-name=") {
          BookName = type.substr(6);
          if (BookName.empty()) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          if (BookName.front() == '"' && BookName.back() == '"') {
            BookName = BookName.substr(1, BookName.length() - 2);
          }
          bookstore.Modify("", BookName);
        } else if (type.substr(0, 8) == "-author=") {
          Author = type.substr(8);
          if (Author.empty()) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          if (Author.front() == '"' && Author.back() == '"') {
            Author = Author.substr(1, Author.length() - 2);
          }
          bookstore.Modify("", "", Author);
        } else if (type.substr(0, 9) == "-keyword=") {
          Keywords = type.substr(9);
          if (Keywords.empty()) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          if (Keywords.find("||") != std::string::npos ||
              Keywords.front() == '|' || Keywords.back() == '|') {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          if (Keywords.front() == '"' && Keywords.back() == '"') {
            Keywords = Keywords.substr(1, Keywords.length() - 2);
          }
          bookstore.Modify("", "", "", Keywords);
        } else if (type.substr(0, 7) == "-price=") {
          std::string PriceStr = type.substr(7);
          if (PriceStr.empty()) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          try {
            Price = std::stof(PriceStr);
            if (Price < 0) {
              std::cout << "Invalid" << std::endl;
              continue;
            }
          } catch (const std::exception &) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          bookstore.Modify("", "", "", "", Price);
        } else {
          std::cout << "Invalid" << std::endl;
          continue;
        }
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
          std::cout << "Invalid" << std::endl;
          continue;
        }
        if (iss >> op) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Show(ISBN);
      } else if (type.substr(0, 6) == "-name=") {
        std::string BookName = type.substr(6);
        if (BookName.empty()) {
          std::cout << "Invalid" << std::endl;
          continue;
        }

        if (iss >> op) {
          std::cout << "Invalid" << std::endl;
          continue;
        }

        if (BookName.front() == '"' && BookName.back() == '"') {
          BookName = BookName.substr(1, BookName.length() - 2);
        }
        bookstore.Show("", BookName);
      } else if (type.substr(0, 8) == "-author=") {
        std::string Author = type.substr(8);
        if (Author.empty()) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        if (iss >> op) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        if (Author.front() == '"' && Author.back() == '"') {
          Author = Author.substr(1, Author.length() - 2);
        }
        bookstore.Show("", "", Author);
      } else if (type.substr(0, 9) == "-keyword=") {
        std::string Keyword = type.substr(9);
        if (Keyword.empty()) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        if (iss >> op) {
          std::cout << "Invalid" << std::endl;
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
            std::cout << "Invalid" << std::endl;
            continue;
          } else if (iss >> op) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          bookstore.ShowFinance(count);
        } else {
          if (iss >> op) {
            std::cout << "Invalid" << std::endl;
            continue;
          }
          bookstore.ShowFinance();
        }
      } else {
        std::cout << "Invalid" << std::endl;
      }
    } else if (op == "buy") {
      std::string ISBN;
      int quantity;
      if (!(iss >> ISBN)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (!(iss >> quantity)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (iss >> op) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Buy(ISBN, quantity);
    } else if (op == "import") {
      float quantity, costPrice;
      if (!(iss >> quantity)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (!(iss >> costPrice)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (iss >> op) {
        std::cout << "Invalid" << std::endl;
        ;
        continue;
      }
      bookstore.Import(quantity, costPrice);
    } else {
      std::cout << "Invalid" << std::endl;
    }
  }
  return 0;
}