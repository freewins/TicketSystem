//
// Created by Freewings on 2025/5/19.
//

#ifndef FILE_HPP
#define FILE_HPP
#include<fstream>
#include<string>

namespace file {
  template<class T>
  class MyFile {
  private:
    const std::string path;
    std::fstream file;

  public:
    MyFile(const std::string &path_);

    ~MyFile();

    void read(long long pos, T &data);

    void update(long long pos, T & data);

    long long getPos();

    long long write(const T &data);
  };

  template<class T>
  MyFile<T>::MyFile(const std::string &path_): path(path_) {
    file.open(path, std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      file.open(path, std::ios::out | std::ios::binary);
    }
  }

  template<class T>
  void MyFile<T>::read(long long pos, T & data) {
    file.seekg(pos);
    file.read(reinterpret_cast<char *>(&data), sizeof(data));
  }

  template<class T>
  long long MyFile<T>::write(const T &data) {
    file.seekg(0, std::ios::end);
    long long pos = file.tellp();
    file.seekp(pos);
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    return pos;
  }

  template<class T>
  long long MyFile<T>::getPos() {
    file.seekg(0, std::ios::end);
    long long pos = file.tellp();
    return pos;
  }

  template<class T>
  void MyFile<T>::update(long long pos, T &data) {
    file.seekp(pos);
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
  }


  template<class T>
  MyFile<T>::~MyFile() {
    file.close();
  }
}
#endif //FILE_HPP
