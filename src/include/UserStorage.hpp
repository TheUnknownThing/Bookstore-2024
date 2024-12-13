#ifndef USERSSTORAGE_HPP
#define USERSSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>

class User {
private:
  struct UserInfo {
    int Privilege;
    char UserID[31];
    char PassWord[31];
    char UserName[31];
  };

  FileOperation<UserInfo> userFile;

public:
  User() {
    userFile.initialise("user.dat");
    FileInit();
  }

  void FileInit() {
    if (userFile.isEmpty()) {
      // ASK USER TO INPUT ADMINISTRATOR INFORMATION
      // TODO
    }
  }

  int GetUserPrivilege(const std::string &UserID) {
    
  }

};

#endif // UNROLLEDLIST_HPP