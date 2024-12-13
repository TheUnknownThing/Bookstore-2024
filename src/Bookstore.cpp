#include <iostream>
#include <string>

int main(){
    std::string op;
    while (true){
        std::cin >> op;
        if (op == "exit" || op == "quit"){
            break;
        }
        else if (op == "insert"){
            std::string index;
            int value;
            std::cin >> index >> value;
        }
        else if (op == "delete"){
            std::string index;
            int value;
            std::cin >> index >> value;
        }
        else if (op == "find"){
            std::string index;
            std::cin >> index;
        }
        else if (op == "print"){
        }
    }
}