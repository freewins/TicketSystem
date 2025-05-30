#include<iostream>
#include<string>
#include "./src/command.hpp"
int main() {
  std::string input;
  bool exit_flag = false;
  command::Command command_;
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  while (!exit_flag) {
    getline(std::cin, input);
    command_.execute(input,exit_flag);
  }
  return 0;
}