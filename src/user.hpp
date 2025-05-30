//
// Created by Freewings on 2025/4/11.
//

#ifndef USER_HPP
#define USER_HPP
#include <filesystem>
#include <vector>
#include "../src/map.hpp"
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

      UserName() {
        username[0] = '\0';
      }

      UserName(const char *username_) {
        if (username_ != nullptr) {
          strcpy(username, username_);
        }
      }
    };

    //存储user在bpt中的信息
    struct value_user_ticket {
      // 1 -> success 0 -> peding -1 -> refund
      int timestamp;
      long long pos;

      bool operator<(const value_user_ticket &other) const {
        return timestamp < other.timestamp;
      }

      bool operator==(const value_user_ticket &other) const {
        return timestamp == other.timestamp;
      }

      bool operator!=(const value_user_ticket &other) const {
        return timestamp != other.timestamp;
      }
    };

    //存储user的购票信息
    struct UserTicket {

      int status;
      char train_id[50];
      char startStation[50], endStation[50];
      utils::Time date;
      utils::Time leaving_Time, arriving_Time;
      int num;
      int price; //总票价
      int per_price; //单人价
      int date_num; // 实际对应的票
      UserTicket() {
        startStation[0] = '\0';
        endStation[0] = '\0';
        train_id[0] = '\0';
      }

      UserTicket(int status_,const char *trainId, const char *startStation_, const char *endStation_,
                 const utils::Time &leaving_Time_,
                 const utils::Time &arriving_Time_, const utils::Time &date_, const int num_, const int price_,const int per_price_,int date_num_) {
        status = status_;
        if (trainId != nullptr) {
          strcpy(train_id, trainId);
        }
        if (startStation_ != nullptr) {
          strcpy(startStation, startStation_);
        }
        if (endStation_ != nullptr) {
          strcpy(endStation, endStation_);
        }
        date = date_;
        leaving_Time = leaving_Time_;
        arriving_Time = arriving_Time_;
        num = num_;
        price = price_;
        per_price = per_price_;
        date_num = date_num_;
      }
    };

    struct Data {
      char password[35];
      char name[25]; //Can be chinese
      char mailAddr[35];
      int privilege;
      int id; //唯一标识符

      Data() {
        password[0] = '\0';
        name[0] = '\0';
        mailAddr[0] = '\0';
        privilege = 0;
        id = 0;
      }

      Data(const Data &other) {
        strcpy(password, other.password);
        strcpy(name, other.name);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;
        id = other.id;
      }

      Data(const char *pass, const char *name_, const char *mail, const int &privilege_, const int &id) {
        if (pass != nullptr) {
          strcpy(password, pass);
        }
        if (name_ != nullptr) {
          strcpy(name, name_);
        }
        if (mail != nullptr) {
          strcpy(mailAddr, mail);
        }
        privilege = privilege_;
      }

      bool operator<(const Data &other) const {
        return id < other.id;
      }

      bool operator==(const Data &other) const {
        return id == other.id;
      }

      Data &operator=(const Data &other) {
        strcpy(name, other.name);
        strcpy(password, other.password);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;
        this->id = other.id;
        return *this;
      }
    };

    BPlusTree<Data, UserName> users;
    BPlusTree<value_user_ticket, UserName> tickets_bpt; //存储购票信息
    file::MyFile<UserTicket> tickets_data;
    sjtu::map<UserName, int> login_stack;
    const std::string PATH = "user_info.data";
    const std::string PATH_TICKETS = "user_tickets.bpt";
    const std::string PATH_DATA_TICKETS = "user_tickets.data";
    int total_users = 0;

  public:
    User(): users("user_info.data"), tickets_bpt("user_tickets.bpt"),
            tickets_data("user_tickets.data") {
      login_stack.clear();
    }

    bool add_user(const char *, const char *, const char *, const char *, const char *, const int &);

    bool login(const char *username, const char *password);

    bool logout(const char *username);

    //Output
    bool query_profile(const char *cur_user, const char *username);

    //Output
    bool modify_profile(const char *cur_user, const char *username, const char *password, const char *name,
                        const char *mail, int &privilege);

    // 1 -> success 0 -> pending -1 fail
    int buy_ticket(train::Train &trains, const char *username, const char *TrainID, const char *startStation,
                   const char *endStation, int num, int timestamp, utils::Time date, bool queue);

    bool query_order(const char *user_name);

    bool refund_ticket(train::Train &trains, const char *username_, int k);

    bool clean();
  };

  inline bool User::add_user(const char *cur_username, const char *username, const char *password,
                             const char *tangible_name, const char *mail, const int &privilege) {
    if (users.GetTotal() == 0) {
      //创建第一个用户
      UserName new_user_name(username);

      Data new_user_data(password, tangible_name, mail, 10, 1);
      users.Insert(new_user_name, new_user_data);
      return true;
    } else {
      UserName cur_user(cur_username);
      auto res = login_stack.find(cur_user);
      if (res == login_stack.end() || res->second <= privilege) {
        return false;
      } else {
        UserName new_user_name(username);
        bool repeat = false;
        users.Search(new_user_name, repeat);
        if (repeat) {
          return false;
        }
        Data new_user_data(password, tangible_name, mail, privilege, users.GetTotal() + 1);
        users.Insert(new_user_name, new_user_data);
        return true;
      }
    }
  }

  inline bool User::login(const char *username, const char *password) {
    UserName user_name(username);
    if (!login_stack.empty()) {
      auto res = login_stack.find(user_name);
      if (res != login_stack.end()) {
        return false;
      }
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name, find);
    if (find) {
      if (strcmp(res_find[0].password, password) == 0) {
        login_stack.insert({user_name, res_find[0].privilege});
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
    if (login_stack.empty()) {
      return false;
    }
    UserName user_name(username);
    UserName cur_user_name(cur_user);
    auto res = login_stack.find(cur_user_name);
    if (res == login_stack.end()) {
      return false;
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name, find);
    if (find) {
      if (res_find[0].privilege < res->second || cur_user_name == user_name) {
        std::cout << username << " " << res_find[0].name << " " << res_find[0].mailAddr << " " << res_find[0].privilege
            << "\n";
        return true;
      }
    }
    return false;
  }

  inline bool User::modify_profile(const char *cur_user, const char *username, const char *password, const char *name,
                                   const char *mail, int &privilege) {
    if (login_stack.empty()) {
      return false;
    }
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
      auto tmp = users.Search(user_name, find_);

      if (find_) {
        Data update_user_data(tmp[0]);
        if (update_user_data.privilege < res->second || res->first == user_name) {
          if (password != nullptr && strlen(password) > 0) {
            strcpy(update_user_data.password, password);
          }
          if (name != nullptr && strlen(name) > 0) {
            strcpy(update_user_data.name, name);
          }
          if (mail != nullptr && strlen(mail) > 0) {
            strcpy(update_user_data.mailAddr, mail);
          }
          if (privilege != -1) {
            update_user_data.privilege = privilege;
          }
          if (users.Update(user_name, update_user_data)) {
            std::cout << username << " " << update_user_data.name << " " << update_user_data.mailAddr << " " <<
                update_user_data.privilege
                << "\n";
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      }
      return false;
    }
  }


  inline int User::buy_ticket(train::Train &trains, const char *username, const char *TrainID, const char *startStation,
                              const char *endStation, int num, int timestamp, utils::Time date, bool queue) {
    if (login_stack.empty()) {
      return -1;
    }
    UserName user_name(username);
    bool find = true;

    auto t = login_stack.find(user_name);
    if (t == login_stack.end()) {
      //未登录
      return -1;
    }
    utils::Time leaving_time, arriving_time;
    int price = 0;
    int per_price = 0;
    int date_num;
    int res = trains.BuyTicket(username, TrainID, startStation, endStation, date, num, queue, timestamp, leaving_time,
                               arriving_time, price,per_price,date_num);
    if (res < 0) {
      return -1;
    } else {
      value_user_ticket new_ticket;
      new_ticket.pos = tickets_data.getPos();
      new_ticket.timestamp = timestamp;
      if (tickets_bpt.Insert(user_name, new_ticket)) {
        UserTicket new_user_ticket(res,TrainID, startStation, endStation, leaving_time, arriving_time, date, num, price,per_price,date_num);
        tickets_data.write(new_user_ticket);
        tickets_bpt.Insert(user_name, new_ticket);
        if (res == 1) {
          return price;
        } else {
          return 0;
        }
      } else {
        return -1;
      }
    }
    return -1;
  }

  inline bool User::refund_ticket(train::Train &trains, const char *username_, int k) {
    if (login_stack.empty()) {
      return false;
    }
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
      if (res.size() < k) {
        return false;
      }
      sjtu::vector<utils::transfer_union> change;
      UserTicket new_ticket;
      //倒数第 k 个
      int order = res.size() - k ;
      tickets_data.read(res[order].pos, new_ticket);
      bool queue;
      if (new_ticket.status == 0) {
        queue = true;
      } else if (new_ticket.status == -1) {
        return false;
      }
      else {
        queue = false;
      }
      if (trains.RefundTicket(username_, new_ticket.train_id, new_ticket.startStation, new_ticket.endStation,
                              new_ticket.date_num, res[order].timestamp, new_ticket.num, queue, change)) {
        new_ticket.status = -1; //退票
        // tickets_bpt.Remove(user_name,res[order]);
        // tickets_bpt.Insert(user_name,res[order]);
        tickets_data.update(res[order].pos, new_ticket);
        if (!change.empty()) {
          for (int i = 0; i < change.size(); ++i) {
            auto item = tickets_bpt.Search(change[i].user_name, find);
            if (find) {
              UserTicket tmp;
              for (int j = 0; j < item.size(); ++j) {
                if (item[j].timestamp == change[i].timestamp) {
                  tickets_data.read(item[j].pos, tmp);
                  tmp.status = change[i].status;
                  // tickets_bpt.Remove(change[i].user_name, item[j]);
                  // tickets_bpt.Insert(change[i].user_name, item[j]);
                  tickets_data.update(item[j].pos, tmp);
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
      return false;
    }
  }

  inline bool User::query_order(const char *user_name) {
    if (login_stack.empty()) {
      return false;
    }
    UserName username(user_name);
    auto t = login_stack.find(username);
    if (t == login_stack.end()) {
      //未登录
      return false;
    }
    bool find = false;
    auto res = tickets_bpt.Search(username, find);
    std::cout << res.size() << "\n";
    if (find) {
      UserTicket ticket_data;
      for (int i = res.size() - 1; i >= 0 ; --i) {

        tickets_data.read(res[i].pos, ticket_data);
        if (ticket_data.status == 0) {
          std::cout << "[pending] ";
        } else if (ticket_data.status == 1) {
          std::cout << "[success] ";
        } else if (ticket_data.status == -1) {
          std::cout << "[refunded] ";
        }
        std::cout << ticket_data.train_id << " " << ticket_data.startStation << " " <<
            ticket_data.leaving_Time << " -> " << ticket_data.endStation << " " << ticket_data.arriving_Time << " " <<
            ticket_data.per_price << " " << ticket_data.num << "\n";
      }
    }
    return true;
  }
}

#endif //USER_HPP
