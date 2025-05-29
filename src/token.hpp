//
// Created by Freewings on 2025/4/10.
//

#ifndef TOKEN_HPP
#define TOKEN_HPP

namespace token {
  class TokenScanner {
  private:
    int current = 0;
    std::string token;

  public :
    TokenScanner();

    explicit TokenScanner(std::string &&input);

    void reset_token(std::string &&input);

    bool has_more_token();

    std::string next_token();
  };


  inline TokenScanner::TokenScanner() {
    current = 0;
  }

  inline TokenScanner::TokenScanner(std::string &&input) {
    current = 0;
    token = input;
    if (token[current] == '[') {
      current++;
    }
  }

  inline void TokenScanner::reset_token(std::string &&input) {
    current = 0;
    token = input;
    if (token[current] == '[') {
      current++;
    }
  }

  inline bool TokenScanner::has_more_token() {
    return current < token.size();
  }

  inline std::string TokenScanner::next_token() {
    std::string result;
    int pre = current;
    for (; current < token.size(); ++current) {
      if (token[current] == ' ') {
        break;
      }
    }
    if (token[current - 1] == ']') {
      result = token.substr(pre, current - pre - 1);
      current++;
    } else {
      result = token.substr(pre, current - pre);
      current++;
    }
    return result;
  }
}

#endif //TOKEN_HPP
