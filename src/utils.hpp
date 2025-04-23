//
// Created by Freewings on 25-4-20.
//

#ifndef UTILS_HPP
#define UTILS_HPP
namespace utils {
  struct Time {
    int mon_, day_, hour_, minute_;
    long long cmp;

    Time() {
      mon_ = day_ = hour_ = minute_ = 0;
      cmp = 0;
    }

    Time(int &&mon, int &&day, int &&hour, int &&minute): mon_(mon), day_(day), hour_(hour), minute_(minute) {
      cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
    }

    bool operator<(const Time &rhs) const {
      return this->cmp < rhs.cmp;
    }
  };

  inline int string_to_int(const std::string& num) {
    int res = 0;
    int flag = 1;
    int pos = 0;
    if (num[0] == '-') {
      flag = -1;
      pos ++;
    }
    while (pos < num.size()) {
      res = res * 10 + (num[pos] - '0');
      pos ++;
    }
    return res;
  }

  inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    int pre_pos = 0; int end_pos = 0;
    while (end_pos < s.size()) {
      if (s[end_pos] == delim) {
        elems.push_back(s.substr(pre_pos, end_pos - pre_pos));
        pre_pos = end_pos + 1;
      }
      end_pos++;
    }
    elems.push_back(s.substr(pre_pos, end_pos - pre_pos));
    return elems;
  }

}
#endif //UTILS_HPP
