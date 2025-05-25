//
// Created by Freewings on 25-4-20.
//

#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstring>
#include <iomanip>
#include <regex>
#include<vector>

namespace utils {
  const int mon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  struct Time {
    int mon_, day_, hour_, minute_;
    long long cmp;

    Time() {
      mon_ = day_ = hour_ = minute_ = 0;
      cmp = 0;
    }

    Time(int &mon, int &day, int &hour, int &minute): mon_(mon), day_(day), hour_(hour), minute_(minute) {
      cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
    }

    Time(int mon, int day, int hour, int minute): mon_(mon), day_(day), hour_(hour), minute_(minute) {
      cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
    }

    void swap(Time &a, Time &b) {
      std::swap(a.mon_, b.mon_);
      std::swap(a.day_, b.day_);
      std::swap(a.hour_, b.hour_);
      std::swap(a.minute_, b.minute_);
      std::swap(a.cmp, b.cmp);
    }

    Time &operator=(const Time &other) {
      mon_ = other.mon_;
      day_ = other.day_;
      hour_ = other.hour_;
      minute_ = other.minute_;
      return *this;
    }


    //返回两个日期之间隔了几天 比如 7.1 - 6.30 = 1
    int minus_mon(const Time &max_, const Time &min_) const {
      Time ma, mb;
      if (max_.cmp < min_.cmp) {
        ma = min_;
        mb = max_;
      } else {
        ma = max_;
        mb = min_;
      }
      if (ma.mon_ == mb.mon_) {
        return ma.day_ - mb.day_;
      }
      int ans = 0;
      for (int i = mb.mon_ + 1; i < ma.mon_; i++) {
        ans += mon[i];
      }
      ans += mon[mb.mon_] - min_.day_;
      ans += ma.day_;
    }

    Time &add_min(const int &time) {
      this->minute_ += time;
      while (this->minute_ >= 60) {
        int h = this->minute_ / 60;
        this->minute_ = this->minute_ % 60;
        this->hour_ += h;
      }
      while (this->hour_ >= 24) {
        int d = this->hour_ / 24;
        this->hour_ = this->hour_ % 24;
        this->day_ += d;
      }
      while (this->day_ >= mon[this->mon_]) {
        this->day_ -= mon[this->mon_];
        this->mon_++;
      }
      this->cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
      return *this;
    }


    bool operator<(const Time &rhs) const {
      return this->cmp < rhs.cmp;
    }
  };
  std::ostream &operator<<(std::ostream &os,const Time &t) {
    os<<std::setw(2)<<std::setfill('0')<<t.mon_<<"-"<<std::setw(2)<<std::setfill('0')<<t.day_<<" ";
    os<<std::setw(2)<<std::setfill('0')<<t.hour_<<":"<<std::setw(2)<<std::setfill('0')<<t.minute_;
    return os;
  }
  //用于user 和 train 在修改队列后的信息交换
  struct transfer_union {
    char user_name[50];
    int timestamp;
    int status;

    transfer_union(const char *username, int timestamp_, int status_) {
      strcpy(user_name, username);
      timestamp = timestamp_;
      status = status_;
    }
  };

  inline int string_to_int(const std::string &num) {
    int res = 0;
    int flag = 1;
    int pos = 0;
    if (num[0] == '-') {
      flag = -1;
      pos++;
    }
    while (pos < num.size()) {
      res = res * 10 + (num[pos] - '0');
      pos++;
    }
    return res;
  }

  inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    int pre_pos = 0;
    int end_pos = 0;
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

  inline void split_int(int *des, const std::string &s, char delim) {
    std::vector<std::string> elems = split(s, delim);
    for (int i = 0; i < elems.size(); i++) {
      des[i] = (string_to_int(elems[i]));
    }
  }

  // op = 1 : only hour and min |  op = 2 : only mon and day
  inline utils::Time string_to_time(const std::string &num, int op) {
    if (op == 1) {
      int pos = num.find(':', 0);
      int hour = string_to_int(num.substr(0, pos));
      int minute = string_to_int(num.substr(pos + 1));
      return Time(0, 0, hour, minute);
    }
    if (op == 2) {
      int pos = num.find('-', 0);
      int mon = string_to_int(num.substr(0, pos));
      int day = string_to_int(num.substr(pos + 1));
      return Time(mon, day, 0, 0);
    }
  }
}
#endif //UTILS_HPP
