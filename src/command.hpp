//
// Created by Freewings on 25-4-20.
//

#ifndef COMMAND_HPP
#define COMMAND_HPP
#define COMMAND_HPP
#include<vector>
#include "../src/vector.hpp"
#include "../src/user.hpp"
#include "../src/train.hpp"
#include "../src/token.hpp"
#include "../src/utils.hpp"

namespace command {
  class Command {
    int timestamp;
    token::TokenScanner token_scanner;
    sjtu::vector<std::string> tokens;
    user::User user;
    train::Train train;

  public:
    Command();

    ~Command();

    void execute(std::string &command, bool &exit_flag);
  };

  inline Command::Command() {
    timestamp = 0;
    if (!tokens.empty()) {
      tokens.clear();
    }
  }

  //false exit  else continue
  inline void Command::execute(std::string &command, bool &exit_flag) {
    token_scanner.reset_token(command);
    tokens.clear();
    int pos = 0;
    while (token_scanner.has_more_token()) {
      tokens.push_back(token_scanner.next_token());
    }
    pos++;
    timestamp = utils::string_to_int(tokens[0]);
    try {
      std::cout << "[" << timestamp << "]" << " ";
      if (tokens[1] == "add_user" && tokens.size() == 14) {
        std::string params[6]{};
        // 0-> cur_username 1->username 2-> password 3-> name 4-> mailAddr 5-> privilege
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-c") {
            params[0] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-u") {
            params[1] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-p") {
            params[2] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-n") {
            params[3] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-m") {
            params[4] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-g") {
            params[5] = tokens[i + 1];
            continue;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        bool res = user.add_user(params[0].c_str(), params[1].c_str(), params[2].c_str(), params[3].c_str(),
                                 params[4].c_str(), utils::string_to_int(params[5]));
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "login" && tokens.size() == 6) {
        std::string params[6]{};
        // 0 -> username 1 -> password
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-p") {
            params[1] = tokens[i + 1];
            continue;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        bool res = user.login(params[0].c_str(), params[1].c_str());
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "logout" && tokens.size() == 4) {
        std::string params[4]{};
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i + 1];
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        bool res = user.logout(params[0].c_str());
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "query_profile" && tokens.size() == 6) {
        std::string params[6]{};
        // 0-> cur_username u -> usrname
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-c") {
            params[0] = tokens[i + 1];
          } else if (tokens[i] == "-u") {
            params[1] = tokens[i + 1];
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        bool res = user.query_profile(params[0].c_str(), params[1].c_str());
        if (!res) {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "modify_profile") {
        std::string params[6]{};

        bool exists[6]{false};
        // 0 - cur_username 1 - username 2 - password -3 name -4 mailaddress -g priviledge
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-c") {
            params[0] = tokens[i + 1];
            exists[0] = true;
            continue;
          } else if (tokens[i] == "-u") {
            params[1] = tokens[i + 1];
            exists[1] = true;
            continue;
          } else if (tokens[i] == "-p") {
            params[2] = tokens[i + 1];
            exists[2] = true;
            continue;
          } else if (tokens[i] == "-n") {
            params[3] = tokens[i + 1];
            exists[3] = true;
            continue;
          } else if (tokens[i] == "-m") {
            params[4] = tokens[i + 1];
            exists[4] = true;
            continue;
          } else if (tokens[i] == "-g") {
            params[5] = tokens[i + 1];
            exists[5] = true;
            continue;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        if (exists[0] && exists[1]) {
          int p = -1;
          if (exists[5]) {
            p = utils::string_to_int(params[5]);
          }
          bool res = user.modify_profile(params[0].c_str(), params[1].c_str(), params[2].c_str(), params[3].c_str(),
                                         params[4].c_str(), p);
          if (!res) {
            std::cout << "-1\n";
            return;
          }
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "add_train" && tokens.size() == 22) {
        std::string params[10] = {};
        //0 -> trainID 1 -> stationNum 2 -> seatNum 3-> stations 4 - > prices
        //5-> startTIme 6- > travelTimes 7 -> stopoverTimes
        //8 -> saleDate 9 -> type
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-i") {
            params[0] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-n") {
            params[1] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-m") {
            params[2] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-s") {
            params[3] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-p") {
            params[4] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-x") {
            params[5] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-t") {
            params[6] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-o") {
            params[7] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-d") {
            params[8] = tokens[i + 1];
            continue;
          } else if (tokens[i] == "-y") {
            params[9] = tokens[i + 1];
            continue;
          } else {
            std::cout << "-1\n";
            return;
          }
        }

        int stationNnum = utils::string_to_int(params[1]);
        int seatNum = utils::string_to_int(params[2]);
        bool res = train.add_train(params[0], stationNnum, seatNum, params[3], params[4], params[5], params[6],
                                  params[7], params[8], params[9][0]);
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "query_ticket" && (tokens.size() == 8 || tokens.size() == 10)) {
        std::string params[4] = {};
        // 0 -> strtStation 1 -> toStation 2 -> date (3 -> p sort order)
        bool exists[4]{false};
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-s") {
            params[0] = tokens[i + 1];
            exists[0] = true;
          } else if (tokens[i] == "-t") {
            params[1] = tokens[i + 1];
            exists[1] = true;
          } else if (tokens[i] == "-d") {
            params[2] = tokens[i + 1];
            exists[2] = true;
          } else if (tokens[i] == "-p") {
            params[3] = tokens[i + 1];
            exists[3] = true;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        int mode = 0;
        if (exists[3]) {
          if (params[3] == "cost") {
            mode = 0;
          } else if (params[3] == "time") {
            mode = 1;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        train.query_ticket(params[0].c_str(), params[1].c_str(), utils::string_to_time(params[2], 2), mode);
      } else if (tokens[1] == "query_transfer" && (tokens.size() == 8 || tokens.size() == 10)) {
        std::string params[4] = {};
        // 0 -> strtStation 1 -> toStation 2 -> date (3 -> p sort order)
        bool exists[4]{false};
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-s") {
            params[0] = tokens[i + 1];
            exists[0] = true;
          } else if (tokens[i] == "-t") {
            params[1] = tokens[i + 1];
            exists[1] = true;
          } else if (tokens[i] == "-d") {
            params[2] = tokens[i + 1];
            exists[2] = true;
          } else if (tokens[i] == "-p") {
            params[3] = tokens[i + 1];
            exists[3] = true;
          } else {
            std::cout << "0\n";
            return;
          }
        }
        int mode = 0;
        if (exists[3]) {
          if (params[3] == "cost") {
            mode = 0;
          } else if (params[3] == "time") {
            mode = 1;
          } else {
            std::cout << "0\n";
            return;
          }
        }
        train.query_transfer(params[0].c_str(), params[1].c_str(), utils::string_to_time(params[2], 2), mode);
      } else if (tokens[1] == "buy_ticket" && (tokens.size() == 14 || tokens.size() == 16)) {
        std::string params[8] = {};
        //0 -> username 1 -> trrainID -2 date -3 fraomSation -4 toStation -5 num -6 queue
        bool exits[8]{false};
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i + 1];
            exits[0] = true;
          } else if (tokens[i] == "-i") {
            params[1] = tokens[i + 1];
            exits[1] = true;
          } else if (tokens[i] == "-d") {
            params[2] = tokens[i + 1];
            exits[2] = true;
          } else if (tokens[i] == "-f") {
            params[3] = tokens[i + 1];
            exits[3] = true;
          } else if (tokens[i] == "-t") {
            params[4] = tokens[i + 1];
            exits[4] = true;
          } else if (tokens[i] == "-n") {
            params[5] = tokens[i + 1];
            exits[5] = true;
          } else if (tokens[i] == "-q") {
            params[6] = tokens[i + 1];
            exits[6] = true;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        bool queue = false;
        if (exits[6]) {
          if (params[6] == "true") {
            queue = true;
          } else if (params[6] == "false") {
            queue = false;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        int num = utils::string_to_int(params[5]);
        int res = user.buy_ticket(train, params[0].c_str(), params[1].c_str(), params[3].c_str(), params[4].c_str(),
                                  num, timestamp, utils::string_to_time(params[2], 2), queue);
        if (res == 0) {
          std::cout << "queue\n";
        } else if (res > 0) {
          std::cout << res << "\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "refund_ticket" && (tokens.size() == 4 || tokens.size() == 6)) {
        std::string params[2] = {};
        bool exits[2];
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-u") {
            params[0] = tokens[i + 1];
            exits[0] = true;
          } else if (tokens[i] == "-n") {
            params[1] = tokens[i + 1];
            exits[1] = true;
          } else {
            std::cout << "-1\n";
            return;
          }
        }
        if (exits[0]) {
          int n = 1; //默认为 1
          if (exits[1]) {
            //输入第i 条消息
            n = utils::string_to_int(params[1]);
          }
          bool res = user.refund_ticket(train, params[0].c_str(), n);
          if (res) {
            std::cout << "0\n";
          } else {
            std::cout << "-1\n";
          }
        } else {
          std::cout << "-1\n";
          return;
        }
      } else if (tokens[1] == "query_order" && tokens.size() == 4) {
        std::string user_name;
        user_name[0] = '\0';
        if (tokens[2] == "-u") {
          user_name = tokens[3];
        } else {
          std::cout << "-1\n";
          return;
        }
        bool res = user.query_order(user_name.c_str());
        if (!res) {
          std::cout << "-1\n";
          return;
        }
      } else if (tokens[1] == "query_train" && tokens.size() == 6) {
        std::string params[2]{};
        // 0 -> trainID 1 -> date
        for (int i = 2; i < tokens.size(); i += 2) {
          if (tokens[i] == "-i") {
            params[0] = tokens[i + 1];
          } else if (tokens[i] == "-d") {
            params[1] = tokens[i + 1];
          } else {
            std::cout << "-1\n";
          }
        }
        bool res = train.query_train(params[0].c_str(), utils::string_to_time(params[1], 2));
        if (!res) {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "release_train") {
        std::string trainID;
        trainID[0] = '\0';
        if (tokens[2] == "-i") {
          trainID = tokens[3];
        } else {
          std::cout << "-1\n";
        }
        bool res = train.release_train(trainID);
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "delete_train" && tokens.size() == 4) {
        std::string trainID;
        trainID[0] = '\0';
        if (tokens[2] == "-i") {
          trainID = tokens[3];
        } else {
          std::cout << "-1\n";
        }
        bool res = train.delete_train(trainID);
        if (res) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      } else if (tokens[1] == "exit" && tokens.size() == 2) {
        std::cout << "bye\n";
        exit_flag = true;
        return;
      } else if (tokens[1] == "clean" && tokens.size() == 2) {
        bool user_clear = user.clean();
        bool train_clear = train.clean();
        return;
      }
    } catch (...) {
      std::cout << timestamp << "\n";
    }
    return;
  }

  inline Command::~Command() {
  }
}
#endif //COMMAND_HPP
