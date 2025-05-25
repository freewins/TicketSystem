//
// Created by Freewings on 25-4-20.
//

#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "../src/vector.hpp"
#include "../src/user.hpp"
#include "../src/train.hpp"
#include "../src/token.hpp"
#include "../src/utils.hpp"

namespace command {
  class Command {
    enum Commands {
      add_user, login, logout, query_profile, modify_profile, query_train, query_ticket,
      query_transfer, buy_ticket, refund_ticket, query_order, add_train, release_train, delete_train, exit, clean
    };

    int timestamp;
    token::TokenScanner token_scanner;
    sjtu::vector<std::string> tokens;
    sjtu::vector<std::string> params;
    user::User user;
    train::Train train;
  public:
    Command();

    bool execute(std::string &&command);

    Commands check_command(std::string &command);
  };

  inline Command::Command() {
    timestamp = 0;
    tokens.clear();
  }

  //false exit  else continue
  inline bool Command::execute(std::string &&command) {
    token_scanner.reset_token(std::move(command));
    tokens.clear();
    int pos = 0;
    while (token_scanner.has_more_token()) {
      tokens.push_back(token_scanner.next_token());
    }
    pos++;
    timestamp = utils::string_to_int(tokens[0]);
    std::cout << timestamp <<" ";
     if (tokens[1] == "add_user"&& tokens.size() == 14 ) {
       std::string params[6];
       // 0-> cur_username 1->username 2-> password 3-> name 4-> mailAddr 5-> privilege
        for (int i = 2;i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-c") {
           params[0] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-u") {
            params[1] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-p") {
            params[2] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-n") {
            params[3] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-m" ) {
            params[4] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-g") {
            params[5] = tokens[i+1];
            continue;
          }
          else {
            //TODO 失败
          }
        }
     }
     else if (tokens[1] == "login" && tokens.size() == 6 ) {
        std::string params[6];
       // 0 -> username 1 -> password
        for (int i = 2; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-p") {
            params[1] = tokens[i+1];
            continue;
          }
          else {
            //TODO 失败
          }
        }
     }
      else if (tokens[1] == "logout" && tokens.size() == 4 ) {
        std::string params[4];
        for (int i = 2; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i+1];
          }
          else {
            //TODO 异常
          }
        }
      }
      else if (tokens[1] == "query_profile" && tokens.size() == 6 ) {
        std::string params[6];
        // 0-> cur_username u -> usrname
        for (int i = 2; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-c") {
            params[0] = tokens[i+1];
          }
          else if (tokens[i] == "-u") {
            params[1] = tokens[i+1];
          }
          else {
            //TODO 异常
          }
        }
      }
      else if (tokens[1] == "modify_profile") {
        std::string params[6];
        // 0 - cur_username 1 - username 2 - password -3 name -4 mailaddress -g priviledge
        for (int i = 2; i <= tokens.size(); i +=2 ) {
          if (tokens[i] == "-c") {
            params[0] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-u") {
            params[1] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-p") {
            params[2] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-n") {
            params[3] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-m") {
            params[4] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-g") {
            params[5] = tokens[i+1];
            continue;
          }
          else {
            //TODO 异常
          }
        }
      }
      else if (tokens[1] == "add_train" && tokens.size() == 22) {
        std::string params[10];
        //0 -> trainID 1 -> stationNum 2 -> setNum 3-> stations 4 - > prices
        //5-> startTIme 6- > travelTimes 7 -> stopoverTimes
        //8 -> saleDate 9 -> type
        for (int i = 2; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-i") {
            params[0] = tokens[i+1];
            continue;
          }
          else if (tokens[i] =="-n") {
            params[1] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-m") {
            params[2] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-s") {
            params[3] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-p") {
            params[4] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-x") {
            params[5] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-t") {
            params[6] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-o") {
            params[7] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-d") {
            params[8] = tokens[i+1];
            continue;
          }
          else if (tokens[i] == "-y") {
            params[9] = tokens[i+1];
            continue;
          }
          else {
            //TODO Throw
          }
        }
        // TODO
      }
      else if (tokens[1] == "query_ticket" && (tokens.size() == 8 || tokens.size() == 10)) {
        std::string params[4];
        // 0 -> strtStation 1 -> toStation 2 -> date (3 -> p sort order)
        bool exists[4]{false};
        for (int i = 2; i< tokens.size(); i+=2) {
          if (tokens[i] == "-s") {
            params[0] = tokens[i+1];
            exists[0] = true;
          }
          else if (tokens[i] == "-t") {
            params[1] = tokens[i+1];
            exists[1] = true;
          }
          else if (tokens[i] == "-d") {
            params[2] = tokens[i+1];
            exists[2] = true;
          }
          else if (tokens[i] == "-p") {
            params[3] = tokens[i+1];
            exists[3] = true;
          }
          else {
            //TODO 异常
          }
        }
        int mode = 0;
        if (exists[3]) {
          if (tokens[3] == "price") {
            mode = 1;
          }

        }
      }
      else if (tokens[1] == "query_transfer" &&  (tokens.size() == 8 || tokens.size() == 10)) {
        std::string params[4];
        // 0 -> strtStation 1 -> toStation 2 -> date (3 -> p sort order)
        bool exists[4]{false};
        for (int i = 2; i< tokens.size(); i+=2) {
          if (tokens[i] == "-s") {
            params[0] = tokens[i+1];
            exists[0] = true;
          }
          else if (tokens[i] == "-t") {
            params[1] = tokens[i+1];
            exists[1] = true;
          }
          else if (tokens[i] == "-d") {
            params[2] = tokens[i+1];
            exists[2] = true;
          }
          else if (tokens[i] == "-p") {
            params[3] = tokens[i+1];
            exists[3] = true;
          }
          else {
            //TODO 异常
          }
        }
      }
      else if (tokens[1] == "buy_ticket" && (tokens.size() ==  14 || tokens.size() == 16)) {
        std::string params[8];
        //0 -> username 1 -> trrainID -2 date -3 fraomSation -4 toStation -5 num -6 queue
        bool exits[8]{false};
        for (int i = 2; i< tokens.size(); i+=2) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i+1];
            exits[0] = true;
          }
          else if (tokens[i] == "-i") {
            params[1] = tokens[i+1];
            exits[1] = true;
          }
          else if (tokens[i] == "-d") {
            params[2] = tokens[i+1];
            exits[2] = true;
          }
          else if (tokens[i] == "-f") {
            params[3] = tokens[i+1];
            exits[3] = true;
          }
          else if (tokens[i] == "-t") {
            params[4] = tokens[i+1];
            exits[4] = true;
          }
          else if (tokens[i] == "-n") {
            params[5] = tokens[i+1];
            exits[5] = true;
          }
          else if (tokens[i] == "-q") {
            params[6] = tokens[i+1];
            exits[6] = true;
          }
          else {
            //TODO
          }
          bool queue = false;
          if (exits[6] ) {
            if (params[6] == "true") {
              queue = true;
            }
            else {
               //TODO 异常
            }
          }
        }
      }
      else if (tokens[1] == "refund_ticket" && (tokens.size() == 4 || tokens.size() == 6)) {
        std::string params[2];
        bool exits[2];
        for (int i = 0; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i+1];
            exits[0] = true;
          }
          else if (tokens[i] == "-n") {
            params[1] = tokens[i+1];
            exits[1] = true;
          }
          else {
            //TODO
          }
        }
        if (exits[0]) {
          int n = 1; //默认为 1
          if (exits[1]) {
            //输入第i 条消息

          }
          else {

          }
        }
        else {
          //TODO
        }
      }
      else if (tokens[1] == "query_order" && tokens.size() == 4) {
        std::string user_name;
        if (tokens[2] == "-u") {
          user_name = tokens[3];
        }
        else {
          //TODO Throw
        }

      }
      else if (tokens[1] == "query_train" && tokens.size() == 6) {
        std::string params[2];
        // 0 -> trainID 1 -> date
        for (int i = 0; i < tokens.size(); i +=2 ) {
          if (tokens[i] == "-i") {
            params[0] = tokens[i+1];
          }
          else if (tokens[i] == "-d") {
            params[1] = tokens[i+1];
          }
          else {
            //TODO throw
          }
        }
      }
      else if (tokens[1] == "release_train") {
        std::string trainID;
        if (tokens[2] == "-i") {
          trainID = tokens[3];
        }
        else {
          //TODO 异常
        }
        //TODO
      }
      else if (tokens[1] == "delete_train" && tokens.size() == 4) {
        std::string trainID;
        if (tokens[2] == "-i") {
          trainID = tokens[3];
        }
        else {
          //TODO 异常
        }
        //TODO

      }
      else if (tokens[1] == "exit" && tokens.size() == 2) {

      }
      else if (tokens [1]== "clean" && tokens.size() == 2) {
        user.clean();
        train.clean();
      }
    return true;
  }
}
#endif //COMMAND_HPP
