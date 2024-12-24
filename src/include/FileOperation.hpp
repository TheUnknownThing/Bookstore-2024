#ifndef FILE_OPERATION_HPP
#define FILE_OPERATION_HPP

#include <fstream>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T> class FileOperation {
private:
  fstream file;
  string file_name;
  int sizeofT = sizeof(T);

public:
  FileOperation() = default;

  ~FileOperation() { file.close(); }

  FileOperation(const string &file_name) : file_name(file_name) {}

  void initialise(string FN = "") {
    this->file_name = FN;
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
      file.clear();
      file.open(file_name, std::ios::out | std::ios::binary);
      file.close();
      file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  //在文件合适位置写入类对象t，并返回写入的位置索引index
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  int write(T &t) {
    file.seekp(0, std::ios::end);
    int index = file.tellp();
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    return index;
  }

  void write(T &t, const int index) {
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
  }

  //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, const int index) {
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index) {
    file.seekg(index);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
  }

  bool isEmpty() {
    file.seekg(0, std::ios::end);
    return file.tellg() == 0;
  }

  int getSize() {
    file.seekg(0, std::ios::end);
    return file.tellg();
  }

  void getLast(T &t) {
    file.seekg(-sizeofT, std::ios::end);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
  }

  int getLastPos() { return (getSize() - sizeofT); }

  //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  void Delete(int index) {
    /* your code here */
    /*file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    for (int i = index + 1; i < cur_idx; ++i) {
        T tmp;
        read(tmp, i);
        update(tmp, i - 1);
    }
    cur_idx--;
    file.close();*/
  }
};

#endif // FILE_OPERATION_HPP