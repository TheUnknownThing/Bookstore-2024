#include "BookstoreOperation.hpp"
#include <iostream>
#include <string>

int main(){
    std::string op;
    BookstoreOperation bookstore;
    while (true){
        std::cin >> op;
        if (op == "exit" || op == "quit"){
            exit(0);
        }
        else if (op == "su"){
            bookstore.Login();
        }
        else if (op == "logout"){
            bookstore.Logout();
        }
        else if (op == "useradd"){
            bookstore.UserAdd();
        }
        else if (op == "register"){
            bookstore.Register();
        }
        else if (op == "passwd"){
            bookstore.Passwd();
        }
        else if (op == "delete"){
            bookstore.DeleteUser();
        }
        else if (op == "buy"){
            bookstore.Buy();
        }
        else if (op == "select"){
            bookstore.Select();
        }
        else if (op == "modify"){
            bookstore.Modify();
        }
        else if (op == "import"){
            bookstore.Import();
        }
        else if (op == "showfinance"){
            bookstore.ShowFinance();
        }
        else if (op == "reportfinance"){
            bookstore.ReportFinance();
        }
        else if (op == "reportemployee"){
            bookstore.ReportEmployee();
        }
        else{
            std::cout << "Invalid" << std::endl;
        }
    }
}