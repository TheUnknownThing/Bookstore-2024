#ifndef USERSTORAGE_HPP
#define USERSTORAGE_HPP

#include "FileOperation.hpp"
#include <cstring>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#define BLOCK_SIZE 600

class User {
private:
  struct UserInfo {
    int Privilege;
    char UserID[31];
    char PassWord[31];
    char UserName[31];
  };

  struct Node {
    int curPos;
    int curSize;
    int nextPos;
    UserInfo firstUser;
  };

  FileOperation<Node> nodeFile;
  FileOperation<UserInfo[BLOCK_SIZE]> userFile;
  UserInfo currentUser;
  std::vector<UserInfo> LoggedInUsers;
  std::stack<std::string> UserSelections;

public:
  User() {
    userFile.initialise("user.dat");
    nodeFile.initialise("userNode.dat");
    FileInit();
  }

  void FileInit() {
    if (userFile.isEmpty()) {
      UserInfo users[BLOCK_SIZE];
      for (int i = 0; i < BLOCK_SIZE; ++i) {
        users[i].Privilege = 0;
        strcpy(users[i].UserID, "");
        strcpy(users[i].PassWord, "");
        strcpy(users[i].UserName, "");
      }
      userFile.write(users);
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      memset(&node.firstUser, 0, sizeof(UserInfo));
      nodeFile.write(node);

      UserAdd("root", "sjtu", "root", 7);
    }
    if (nodeFile.isEmpty()) {
      Node node;
      node.curPos = 0;
      node.curSize = 0;
      node.nextPos = -1;
      memset(&node.firstUser, 0, sizeof(UserInfo));
      nodeFile.write(node);
    }
    currentUser.Privilege = 0;
    strcpy(currentUser.UserID, "");
    strcpy(currentUser.PassWord, "");
    strcpy(currentUser.UserName, "");
  }

  void PrintAll() {
    Node cur;
    int nodePos = 0;
    while (true) {
      nodeFile.read(cur, nodePos);
      UserInfo users[BLOCK_SIZE];
      userFile.read(users, cur.curPos);
      for (int i = 0; i < cur.curSize; i++) {
        std::cout << users[i].UserID << " " << users[i].PassWord << " "
                  << users[i].UserName << " " << users[i].Privilege
                  << std::endl;
      }
      std::cout << "Node Information: " << cur.curPos << " " << cur.curSize
                << " " << cur.nextPos << " " << cur.firstUser.UserID << " "
                << std::endl;
      std::cout << "Node End" << std::endl;
      if (cur.nextPos == -1) {
        break;
      }
      nodePos = cur.nextPos;
    }
  }

  bool CompNode(const Node &node, const std::string &UserID) {
    return strcmp(node.firstUser.UserID, UserID.c_str()) < 0;
  }

  std::pair<Node, int> FindNode(const std::string &UserID) {
    Node curNode, nextNode;
    int nodePos = 0;
    nodeFile.read(curNode, nodePos);
    while (true) {
      if (!CompNode(curNode, UserID)) {
        return {curNode, nodePos};
      }
      if (curNode.nextPos == -1) {
        return {curNode, nodePos};
      }
      nodePos = curNode.nextPos;
      nodeFile.read(curNode, nodePos);
    }
  }

  bool ValidateUser(const UserInfo &user) {
    // UserName: all ASCII characters, length <= 30
    if (strlen(user.UserName) > 30) {
      return false;
    }
    // UserID, Password: Numbers, letters, and underline, length <= 30
    if (strlen(user.UserID) > 30 || strlen(user.PassWord) > 30) {
      return false;
    }
    for (int i = 0; i < strlen(user.UserID); ++i) {
      if (!isalnum(user.UserID[i]) &&
          user.UserID[i] != '_') { // isalnum: Numbers and letters
        return false;
      }
    }
    for (int i = 0; i < strlen(user.PassWord); ++i) {
      if (!isalnum(user.PassWord[i]) && user.PassWord[i] != '_') {
        return false;
      }
    }
    return true;
  }

  bool InsertUser(const UserInfo &user) {
    auto [curNode, nodePos] = FindNode(user.UserID);
    UserInfo users[BLOCK_SIZE];
    userFile.read(users, curNode.curPos);

    int i = 0;
    while (i < curNode.curSize && strcmp(users[i].UserID, user.UserID) < 0) {
      i++;
    }
    if (i < curNode.curSize && strcmp(users[i].UserID, user.UserID) == 0) {
      // std::cout << "User already exists" << std::endl; // debug
      return false; // 用户已存在
    }

    for (int j = curNode.curSize; j > i; --j) {
      users[j] = users[j - 1];
    }
    users[i] = user;
    curNode.curSize++;

    curNode.firstUser = users[0];

    if (curNode.curSize < BLOCK_SIZE) {
      userFile.update(users, curNode.curPos);
      nodeFile.update(curNode, nodePos);
    } else {
      Node newNode;
      int mid = curNode.curSize / 2;
      newNode.curSize = curNode.curSize - mid;
      curNode.curSize = mid;

      UserInfo newUsers[BLOCK_SIZE];
      memcpy(newUsers, users + mid, sizeof(UserInfo) * newNode.curSize);

      userFile.update(users, curNode.curPos);
      curNode.firstUser = users[0];

      newNode.curPos = userFile.write(newUsers);
      newNode.firstUser = newUsers[0];
      newNode.nextPos = curNode.nextPos;
      curNode.nextPos = nodeFile.write(newNode);

      nodeFile.update(curNode, nodePos);
    }
    return true;
  }

  void Delete(const std::string &UserID) {
    auto [curNode, nodePos] = FindNode(UserID);
    UserInfo users[BLOCK_SIZE];
    userFile.read(users, curNode.curPos);

    int i = 0;
    while (i < curNode.curSize &&
           strcmp(users[i].UserID, UserID.c_str()) != 0) {
      i++;
    }
    if (i == curNode.curSize) {
      return;
    }

    for (int j = i; j < curNode.curSize - 1; ++j) {
      users[j] = users[j + 1];
    }
    curNode.curSize--;

    if (curNode.curSize > 0) {
      curNode.firstUser = users[0];
    } else {
      memset(&curNode.firstUser, 0, sizeof(UserInfo));
    }

    userFile.update(users, curNode.curPos);
    nodeFile.update(curNode, nodePos);

    if (curNode.curSize < BLOCK_SIZE / 4 && curNode.nextPos != -1) {
      Node nextNode;
      int nextNodePos = curNode.nextPos;
      nodeFile.read(nextNode, nextNodePos);

      if (curNode.curSize + nextNode.curSize < BLOCK_SIZE) {
        UserInfo nextUsers[BLOCK_SIZE];
        userFile.read(nextUsers, nextNode.curPos);

        memcpy(users + curNode.curSize, nextUsers,
               sizeof(UserInfo) * nextNode.curSize);
        curNode.curSize += nextNode.curSize;
        curNode.nextPos = nextNode.nextPos;

        curNode.firstUser = users[0];

        userFile.update(users, curNode.curPos);
        nodeFile.update(curNode, nodePos);

        nodeFile.Delete(nextNodePos);
        userFile.Delete(nextNode.curPos);
      }
    }
    return;
  }

  UserInfo FindUser(const std::string &UserID) {
    auto [curNode, nodePos] = FindNode(UserID);
    UserInfo users[BLOCK_SIZE];
    userFile.read(users, curNode.curPos);

    for (int i = 0; i < curNode.curSize; ++i) {
      if (strcmp(users[i].UserID, UserID.c_str()) == 0) {
        return users[i];
      }
    }
    return UserInfo();
  }

  int GetUserPrivilege(const std::string &UserID) {
    UserInfo user = FindUser(UserID);
    return user.Privilege;
  }

  int GetCurrentUserPrivilege() { return currentUser.Privilege; }

  std::string GetCurrentUserID() { return std::string(currentUser.UserID); }

  bool Login(const std::string &UserID, const std::string &PassWord) {
    UserInfo user = FindUser(UserID);
    if (strcmp(user.UserID, UserID.c_str()) != 0) {
      return false;
    }
    if (PassWord == "") {
      // if current user privilege is higher than the user to be logged in
      if (currentUser.Privilege >= user.Privilege) {
        currentUser = user;
        LoggedInUsers.push_back(user);
        return true;
      }
      return false;
    }
    if (strcmp(user.PassWord, PassWord.c_str()) == 0) {
      currentUser = user;
      LoggedInUsers.push_back(user);
      UserSelections.push("");
      return true;
    }
    return false;
  }

  void Logout() {
    if (LoggedInUsers.empty()) {
      return;
    } else {
      LoggedInUsers.pop_back();
      UserSelections.pop();
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

  bool UserAdd(const std::string &UserID, const std::string &PassWord,
               const std::string &UserName, int Privilege) {
    UserInfo user;
    user.Privilege = Privilege;
    if (user.Privilege >= currentUser.Privilege) {
      return false;
    }
    strcpy(user.UserID, UserID.c_str());
    strcpy(user.PassWord, PassWord.c_str());
    strcpy(user.UserName, UserName.c_str());
    if (!ValidateUser(user)) {
      return false;
    }
    return InsertUser(user);
  }

  bool Register(const std::string &UserID, const std::string &PassWord,
                const std::string &UserName) {
    UserInfo user;
    user.Privilege = 1;
    strcpy(user.UserID, UserID.c_str());
    strcpy(user.PassWord, PassWord.c_str());
    strcpy(user.UserName, UserName.c_str());
    if (!ValidateUser(user)) {
      return false;
    }
    return InsertUser(user);
  }

  // normal user
  bool Passwd(const std::string &UserID, const std::string &PassWord,
              const std::string &NewPassWord) {
    UserInfo user = FindUser(UserID);
    if (strcmp(user.UserID, UserID.c_str()) != 0) {
      return false;
    }
    if (strcmp(user.PassWord, PassWord.c_str()) == 0) {
      strcpy(user.PassWord, NewPassWord.c_str());
      if (!ValidateUser(user)) {
        return false;
      }
      Delete(UserID);
      InsertUser(user);
      return true;
    }
    return false;
  }

  // root
  bool Passwd(const std::string &UserID, const std::string &NewPassWord) {
    UserInfo user = FindUser(UserID);
    if (strcmp(user.UserID, UserID.c_str()) != 0) {
      return false;
    }
    strcpy(user.PassWord, NewPassWord.c_str());
    if (!ValidateUser(user)) {
      return false;
    }
    Delete(UserID);
    InsertUser(user);
    return true;
  }

  bool DeleteUser(const std::string &UserID) {
    UserInfo user = FindUser(UserID);
    if (strcmp(user.UserID, UserID.c_str()) != 0) {
      return false;
    }
    // check if logged in
    for (auto &u : LoggedInUsers) {
      if (strcmp(u.UserID, UserID.c_str()) == 0) {
        return false;
      }
    }
    Delete(UserID);
    return true;
  }


  // the following functions are for Book operation
  std::string getCurrentUserSelection() {
    if (UserSelections.empty()) {
      return "";
    }
    return UserSelections.top();
  }

  void setCurrentUserSelection(const std::string &selection) {
    if (UserSelections.empty()) {
      return;
    }
    UserSelections.pop();
    UserSelections.push(selection);
  }
};

#endif // USERSTORAGE_HPP