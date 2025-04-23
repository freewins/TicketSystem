#include<iostream>
#include<string>
#include "./src/command.hpp"
#include "./src/token.hpp"
#include"./src/user.hpp"
#include "./src/train.hpp"
#include "./src/utils.hpp"
int main() {
  std::string input;
  bool flag = true;
  command::Command command_;
  while (flag) {
    getline(std::cin, input);
    flag = command_.execute(std::move(input));
  }
  return 0;
}