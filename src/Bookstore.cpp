#include "BookstoreOperation.hpp"
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
      exit(0);
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
    } 
    // TODO: Add more operations
    else if (op == "select") {
      std::string ISBN;
      if (!(iss >> ISBN) || (iss >> op)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Select(ISBN);
    } else if (op == "modify") {
      std::string type;
      if (!(iss >> type)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (type == "-ISBN") {
        std::string ISBN;
        if (!(iss >> ISBN) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Modify(ISBN);
      } else if (type == "-name") {
        std::string BookName;
        if (!(iss >> BookName) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Modify("", BookName);
      } else if (type == "-author") {
        std::string Author;
        if (!(iss >> Author) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Modify("", "", Author);
      } else if (type == "-keyword") {
        std::string Keyword;
        if (!(iss >> Keyword) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Modify("", "", "", Keyword);
      } else {
        std::cout << "Invalid" << std::endl;
      }

    } else if (op == "show") {
      std::string type;
      if (!(iss >> type)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      if (type == "-ISBN") {
        std::string ISBN;
        if (!(iss >> ISBN) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Show(ISBN);
      } else if (type == "-name") {
        std::string BookName;
        if (!(iss >> BookName) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Show("", BookName);
      } else if (type == "-author") {
        std::string Author;
        if (!(iss >> Author) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Show("", "", Author);
      } else if (type == "-keyword") {
        std::string Keyword;
        if (!(iss >> Keyword) || (iss >> op)) {
          std::cout << "Invalid" << std::endl;
          continue;
        }
        bookstore.Show("", "", "", Keyword);
      } else {
        std::cout << "Invalid" << std::endl;
      }
    } else if (op == "buy") {
      std::string ISBN;
      int quantity;
      if (!(iss >> ISBN >> quantity) || (iss >> op)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Buy(ISBN, quantity);
    } else if (op == "import") {
      std::string ISBN;
      float quantity, costPrice;
      if (!(iss >> ISBN >> quantity >> costPrice) || (iss >> op)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Import(quantity, costPrice);
    }
    else {
      std::cout << "Invalid" << std::endl;
    }
  }
  return 0;
}