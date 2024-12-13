#ifndef USERSSTORAGE_HPP
#define USERSSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <stack>

class User {
private:
  struct UserInfo {
    int Privilege;
    char UserID[31];
    char PassWord[31];
    char UserName[31];
  };

  struct StoredUserSelection {
    char BookISBN[21];
  };

  FileOperation<UserInfo> userFile;
  UserInfo currentUser;
  std::vector<UserInfo> LoggedInUsers;
  std::stack<StoredUserSelection> UserSelections;

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

  bool UserAdd(const std::string &UserID, const std::string &PassWord, const std::string &UserName, int Privilege) {
    if (GetUserPrivilege(UserID) != 7) {
      return false;
    }
    UserInfo user;
    user.Privilege = Privilege;
    strcpy(user.UserID, UserID.c_str());
    strcpy(user.PassWord, PassWord.c_str());
    strcpy(user.UserName, UserName.c_str());
    userFile.write(user);
    return true;
  }
  
  bool Register() {

  }

  bool DeleteUser() {

  }

  bool Passwd() {

  }

  bool Login(const std::string &UserID, const std::string &PassWord) {
    

  }

  void Logout() {
    if (currentUser.Privilege == 0) {
      return;
    } else {
      LoggedInUsers.pop_back();
      if (LoggedInUsers.empty()) {
        currentUser.Privilege = 0;
        strcpy(currentUser.UserID, "");
        strcpy(currentUser.PassWord, "");
        strcpy(currentUser.UserName, "");
      } else {
        currentUser = LoggedInUsers.back();
      }
    }
  }


};

#endif // UNROLLEDLIST_HPP