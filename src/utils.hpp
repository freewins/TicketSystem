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

    Time (const  Time & mon,const Time & hour) {
      mon_ = mon.mon_;
      day_ = mon.day_;
      hour_ = hour.hour_;
      minute_ = hour.minute_;
      cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
    }
    void UpdateCmp() {
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
      cmp = mon_ * 1e6 + day_ * 1e4 + hour_ * 1e2 + minute_;
      return *this;
    }
    //这个是修改自身的 ，减少对应的天数
    Time & minus_day(int x) {
      this->day_ -= x;
      while (this->day_ <= 0) {
        this->mon_ --;
        this -> day_ += mon[this->mon_];
      }
      this->cmp = this->mon_ * 1e6 + this->day_ * 1e4 + this->hour_ * 1e2 + this->minute_;
      return *this;
    }

    Time & add_day(int x) {
      this->day_ += x;
      while (this->day_ > mon[this->mon_]) {
        this -> day_ -= mon[this->mon_];
        this->mon_ ++;
      }
      this->cmp = this->mon_ * 1e6 + this->day_ * 1e4 + this->hour_ * 1e2 + this->minute_;
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
      ans += mon[mb.mon_] - mb.day_;
      ans += ma.day_;
      return ans;
    }

    void reset_hour(utils::Time hour) {
      this -> hour_ = hour.hour_;
      this->minute_ = hour.minute_;
      this->cmp = this->mon_ * 1e6 +  this->day_ * 1e4 + this->hour_ * 1e2 + this->minute_;
    }
    Time add_min(const int &time) {
      Time res = *this;
      res.minute_ += time;
      while (res.minute_ >= 60) {
        int h = res.minute_ / 60;
        res.minute_ = res.minute_ % 60;
        res.hour_ += h;
      }
      while (res.hour_ >= 24) {
        int d = res.hour_ / 24;
        res.hour_ = res.hour_ % 24;
        res.day_ += d;
      }
      while (res.day_ > mon[res.mon_]) {
        res.day_ -= mon[res.mon_];
        res.mon_++;
      }
      res.cmp = res.mon_ * 1e6 + res.day_ * 1e4 + res.hour_ * 1e2 + res.minute_;
      return res;
    }


    bool operator<(const Time &rhs) const {
      return this->cmp < rhs.cmp;
    }

    bool operator>(const Time &rhs) const {
      return this->cmp > rhs.cmp;
    }
    bool operator<=(const Time &rhs) const {
      return this->cmp <= rhs.cmp;
    }
    bool operator>=(const Time &rhs) const {
      return this->cmp >= rhs.cmp;
    }
    bool operator==(const Time &rhs) const {
      return this->cmp == rhs.cmp;
    }
    bool operator!=(const Time &rhs) const {
      return this->cmp != rhs.cmp;
    }
  };
  //确保 1-> max 2->min
  int operator-(const Time &max, const Time &min) {
    int min_max = 0;
    int min_min = 0;
    for(int i = 1; i < max.mon_;i++){
      min_max += mon[i] * 24 * 60;
    }
    min_max += max.day_ * 24 * 60;
    min_max += max.hour_ * 60;
    min_max += max.minute_;
    for(int i = 1; i < min.mon_;i++){
      min_min += mon[i] * 24 * 60;
    }
    min_min += min.day_ * 24 * 60;
    min_min += min.hour_ * 60;
    min_min += min.minute_;
    return min_max - min_min;
  }
  std::ostream &operator<<(std::ostream &os,const Time &t) {
    os<<std::setw(2)<<std::setfill('0')<<t.mon_<<"-"<<std::setw(2)<<std::setfill('0')<<t.day_<<" ";
    os<<std::setw(2)<<std::setfill('0')<<t.hour_<<":"<<std::setw(2)<<std::setfill('0')<<t.minute_;
    return os;
  }
  //用于user 和 train 在修改队列后的信息交换
  struct transfer_union {
    char user_name[25];
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
    return Time(0, 0, 0, 0);
  }
  //注意，这里面传入的date是这趟列车启动的时间，而非是开始发行的时间
  void getDateNum(const utils::Time date,const utils::Time & start_time,utils::Time & leaving_time,utils::Time & arrving_time,int ltime,int atime,int &  date_num) {
    leaving_time = date;

    leaving_time.hour_ = start_time.hour_;
    leaving_time.minute_ = start_time.minute_;
    arrving_time = leaving_time;
    leaving_time = leaving_time.add_min(ltime);
    arrving_time = arrving_time.add_min(atime);
    int d = date.minus_mon(leaving_time,date);
    if (d > 0) {
      date_num -= d;
      arrving_time.minus_day(d);
      leaving_time.minus_day(d);
    }
  }
}
#endif //UTILS_HPP
