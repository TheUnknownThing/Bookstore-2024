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
      if (!(iss >> userID)) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      iss >> passWord;
      if (iss >> std::ws >> op) {
        std::cout << "Invalid" << std::endl;
        continue;
      }
      bookstore.Login(userID, passWord);
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
      if ((iss >> userID >> passWord >> newPassWord) && !(iss >> op)) {
        bookstore.Passwd(userID, passWord, newPassWord);
      } else if ((iss >> userID >> newPassWord) && !(iss >> op)) {
        bookstore.Passwd(userID, newPassWord);
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
    else {
      std::cout << "Invalid" << std::endl;
    }
  }
  return 0;
}