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
  UserInfo currentUser;

public:
  User() {
    userFile.initialise("user.dat");
    FileInit();
  }

  void FileInit() {
    if (userFile.isEmpty()) {
        UserInfo user;
        user.Privilege = 7;
        strcpy(user.UserID, "root");
        strcpy(user.PassWord, "sjtu");
        strcpy(user.UserName, "root");
        userFile.write(user);
    }
    currentUser.Privilege = 0;
    strcpy(currentUser.UserID, "");
    strcpy(currentUser.PassWord, "");
    strcpy(currentUser.UserName, "");
  }

  int GetUserPrivilege(const std::string &UserID) {
    
  }

  int GetCurrentUserPrivilege() {
    return currentUser.Privilege;
  }

  bool UserAdd() {

  }
  
  bool Register() {

  }

  bool DeleteUser() {

  }

  bool Passwd() {

  }

  bool Login() {

  }

  void Logout() {

  }


};

#endif // UNROLLEDLIST_HPP