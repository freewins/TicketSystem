//
// Created by Freewings on 25-4-20.
//

#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "../src/vector.hpp"

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
    std::vector<std::string> tokens;
    std::vector<std::string> params;

  public:
    Command();

    bool execute(std::string &&command);
    Commands check_command(std::string &command);
  };

  inline Command::Command() {
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

    return true;
  }

}
#endif //COMMAND_HPP
