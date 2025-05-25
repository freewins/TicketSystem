//
// Created by Freewings on 2025/4/11.
//

#ifndef USER_HPP
#define USER_HPP
#include <filesystem>
#include <map> //TODO need to change?
#include "../src/BPlusTree.hpp"
#include "../src/train.hpp"
#include "../src/file.hpp"

namespace user {
  class User {
  private:
    struct UserName {
      char username[25];

      bool operator<(const UserName &other) const {
        return strcmp(username, other.username) < 0;
      }

      bool operator==(const UserName &other) const {
        return strcmp(username, other.username) == 0;
      }

      bool operator!=(const UserName &other) const {
        return strcmp(username, other.username) != 0;
      }

      UserName() = default;

      UserName(const char *username_) {
        strcpy(username, username_);
      }
    };

    //存储user在bpt中的信息
    struct value_user_ticket {
      int status;
      int timestamp;
      long long pos;

      bool operator<(const value_user_ticket &other) const {
        return timestamp < other.timestamp;
      }

      bool operator==(const value_user_ticket &other) const {
        return timestamp == other.timestamp ;
      }

      bool operator!=(const value_user_ticket &other) const {
        return timestamp != other.timestamp ;
      }
    };

    //存储user的购票信息
    struct UserTicket {
      char train_id[50];
      char startStation[50], endStation[50];
      utils::Time date;
      utils::Time leaving_Time, arriving_Time;
      int num;
      int price;

      UserTicket() = default;

      UserTicket(const char *trainId, const char *startStation_, const char *endStation_,
                 const utils::Time &leaving_Time_,
                 const utils::Time &arriving_Time_, const utils::Time &date_, const int num_, const int price_) {
        strcpy(train_id, trainId);
        strcpy(startStation, startStation_);
        strcpy(endStation, endStation_);
        date = date_;
        leaving_Time = leaving_Time_;
        arriving_Time = arriving_Time_;
        num = num_;
        price = price_;
      }
    };

    struct Data {
      char password[30];
      char name[20]; //Can be chinese
      char mailAddr[35];
      int privilege;

      Data() = default;

      Data(const Data &other) {
        strcpy(password, other.password);
        strcpy(name, other.name);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;
      }

      Data(const char *pass, const char *name_, const char *mail, const int &privilege_) {
        strcpy(password, pass);
        strcpy(name, name_);
        strcpy(mailAddr, mail);
        privilege = privilege_;
      }

      bool operator<(const Data &other) const {
        return strcmp(name, other.name);
      }

      bool operator==(const Data &other) const {
        return strcmp(name, other.name) == 0;
      }

      Data &operator=(const Data &other) {
        strcpy(name, other.name);
        strcpy(password, other.password);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;
        return *this;
      }
    };

    BPlusTree<Data, UserName> users;
    BPlusTree<value_user_ticket, UserName> tickets_bpt; //存储购票信息
    file::MyFile<UserTicket> tickets_data;
    std::map<UserName, int> login_stack;
    const std::string PATH = "../db/user_info.data";
    const std::string PATH_TICKETS = "../db/user_tickets.bpt";
    const std::string PATH_DATA_TICKETS = "../db/user_tickets.data";
    int total_users = 0;

  public:
    User(): users(PATH), tickets_bpt(PATH_TICKETS), tickets_data(PATH_DATA_TICKETS) {
    }

    bool add_user(const char *, const char *, const char *, const char *, const char *, const int &);

    bool login(const char *username, const char *password);

    bool logout(const char *username);

    bool query_profile(const char *cur_user, const char *username);

    bool modify_profile(const char *cur_user, const char *username, const char *password, const char *name,
                        const char *mail, int &privilege);

    int buy_ticket(train::Train &trains, const char *username, const char *TrainID, const char *startStation,
                   const char *endStation, int num, int timestamp, utils::Time date, bool queue);

    bool query_order(const char *user_name);

    bool refund_ticket(train::Train &trains, const char *username_, int k);

    bool clean();
  };

  inline bool User::add_user(const char *cur_username, const char *username, const char *password,
                             const char *tangible_name, const char *mail, const int &privilege) {
    UserName cur_user(cur_username);
    auto res = login_stack.find(cur_user);
    if (res == login_stack.end() || res->second <= privilege) {
      return false;
    } else {
      UserName new_user_name(username);
      Data new_user_data(password, tangible_name, mail, privilege);
      users.Insert(new_user_name, new_user_data);
    }
  }

  inline bool User::login(const char *username, const char *password) {
    UserName user_name(username);
    auto res = login_stack.find(user_name);
    if (res == login_stack.end() || res->second <= 0) {
      return false;
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name, find);
    if (find) {
      if (strcmp(res_find[0].password, password) == 0) {
        login_stack.insert(std::make_pair(user_name, res_find[0].privilege));
        return true;
      }
    }
    return false;
  }

  inline bool User::logout(const char *username) {
    UserName user_name(username);
    auto res = login_stack.find(user_name);
    if (res == login_stack.end()) {
      return false;
    } else {
      login_stack.erase(res);
      return true;
    }
  }

  inline bool User::query_profile(const char *cur_user, const char *username) {
    UserName user_name(username);
    UserName cur_user_name(cur_user);
    auto res = login_stack.find(cur_user_name);
    if (res == login_stack.end()) {
      return false;
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name, find);
    if (find) {
      if (res_find[0].privilege <= res->second) {
        std::cout << username << " " << res_find[0].name << " " << res_find[0].mailAddr << " " << res_find[0].privilege
            << "\n";
        return true;
      }
    }
    return false;
  }

  inline bool User::modify_profile(const char *cur_user, const char *username, const char *password, const char *name,
                                   const char *mail, int &privilege) {
    UserName user_name(username);
    UserName cur_user_name(cur_user);
    auto res = login_stack.find(cur_user_name);
    if (res == login_stack.end()) {
      return false;
    }
    if (privilege != -1 && privilege >= res->second) {
      return false;
    } else {
      bool find_ = false;
      Data update_user_data(users.Search(user_name, find_)[0]);
      if (update_user_data.privilege <= res->second) {
        if (password != nullptr) {
          strcpy(update_user_data.password, password);
        }
        if (name != nullptr) {
          strcpy(update_user_data.name, name);
        }
        if (mail != nullptr) {
          strcpy(update_user_data.mailAddr, mail);
        }
        if (privilege != -1) {
          update_user_data.privilege = privilege;
        }
        return users.Update(user_name, update_user_data);
      }
      return false;
    }
  }


  inline int User::buy_ticket(train::Train &trains, const char *username, const char *TrainID, const char *startStation,
                              const char *endStation, int num, int timestamp, utils::Time date, bool queue) {
    UserName user_name(username);
    bool find = false;
    auto t = users.Search(user_name, find)[0];
    if (find) {
      auto t = login_stack.find(user_name);
      if (t == login_stack.end()) {
        //未登录
        return false;
      }
      //TODO 检查目前需要添加什么内容，因为信息里面放的是发车和结束的时间，但是给的只有日期，这时候需要列车返回对应信息
      utils::Time leaving_time, arriving_time;
      int price = 0;
      int res = trains.BuyTicket(username, TrainID, startStation, endStation, date, num, queue, timestamp, leaving_time,
                                 arriving_time, price);
      if (res < 0) {
        // 加入队列
        return -1;
      } else {
        value_user_ticket new_ticket;
        new_ticket.pos = tickets_data.getPos();
        new_ticket.status = res;
        new_ticket.timestamp = timestamp;
        if (tickets_bpt.Insert(user_name, new_ticket)) {
          UserTicket new_user_ticket(TrainID, startStation, endStation, leaving_time, arriving_time, date, num, price);
          tickets_data.write(new_user_ticket);
          tickets_bpt.Insert(user_name, new_ticket);
          return res;
        } else {
          return -1;
        }
      }
    }
    return -1;
  }

  inline bool User::refund_ticket(train::Train &trains, const char *username_, int k) {
    UserName user_name(username_);
    /*
     * const char *TrainID, const char *startStation,
     * const char *endStation, utils::Time date, bool queue
     */
    auto t = login_stack.find(user_name);
    if (t == login_stack.end()) {
      return false;
    }
    bool find = false;
    sjtu::vector<value_user_ticket> res = tickets_bpt.Search(user_name, find);
    if (find) {
      if (res.size() > k) {
        return false;
      }
      sjtu::vector<utils::transfer_union> change;
      UserTicket new_ticket;
      tickets_data.read(res[k - 1].pos, new_ticket);
      bool queue;
      if (res[k - 1].status == 0) {
        queue = true;
      } else {
        queue = false;
      }
      if (trains.RefundTicket(username_, new_ticket.train_id, new_ticket.startStation, new_ticket.endStation,
                              new_ticket.date, res[k - 1].timestamp, new_ticket.num, queue, change)) {
        if (!change.empty()) {
          for (int i = 0;i < change.size(); ++i) {
            auto item = tickets_bpt.Search(change[i].user_name, find);
            if (find) {
              for (int j = 0; j < item.size(); ++j) {
                if (item[j].timestamp == change[i].timestamp) {
                  item[j].status = change[i].status;
                  tickets_bpt.Update(change[i].user_name, item[j]);
                }
              }
            } else {
              throw sjtu::runtime_error();
            }
          }
        }
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }


  inline bool User::clean() {
    std::filesystem::path fPATH = "../db/user_info.data";
    std::filesystem::path fPATH_TICKETS = "../db/user_tickets.bpt";
    std::filesystem::path fPATH_DATA_TICKETS = "../db/user_tickets.data";
    try {
      if (std::filesystem::remove(fPATH) && std::filesystem::remove(fPATH_TICKETS) && std::filesystem::remove(
            fPATH_DATA_TICKETS)) {
        return true;
      } else {
        return false;
      }
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  }
}

#endif //USER_HPP
