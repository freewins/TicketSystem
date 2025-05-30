#include<iostream>
#include<string>
#include "./src/command.hpp"

#ifdef DEBUG
#include<fstream>
#endif
int main() {
#ifdef DEBUG

   std::string paths[] ={
     "station_info.bpt",
     "train_basic_info.bpt",
     "train_stations_info.data",
     "queue_info.bpt",
     "user_info.data",
     "user_tickets.bpt",
     "user_tickets.data"
   };
   for (int i = 0 ; i < 7; i++) {
     if (remove(paths[i].c_str()) == 0) {
       std::cout << "remove ok" << std::endl;
     }
     else{}
   }

   freopen("../testcases/63.in", "r", stdin);
   freopen("./out/63.out.tmp", "w", stdout);
  #endif

  std::string input;
  bool exit_flag = false;
  command::Command command_;
  //std::ios::sync_with_stdio(false);
  //std::cin.tie(nullptr);
  //std::cout.tie(nullptr);
  while (!exit_flag) {
    getline(std::cin, input);
    command_.execute(input,exit_flag);
  }
  return 0;
}