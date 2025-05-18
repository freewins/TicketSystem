//
// Created by Freewings on 2025/4/11.
//

#ifndef USER_HPP
#define USER_HPP
#include <map> //TODO need to change?
#include "../src/BPlusTree.hpp"

namespace user {
  class User {
  private:
    struct UserName {
      char username[25];

      bool operator<(const UserName &other) const {
        return strcmp(username, other.username) < 0;
      }
      UserName() = default;
      UserName (const char * username_) {
        strcpy(username, username_);
      }
    };
    struct Data {
      char password[30];
      char name[20]; //Can be chinese
      char mailAddr[35];
      int privilege;
      Data() = default;
      Data(const Data & other) {
        strcpy(password, other.password);
        strcpy(name, other.name);
        strcpy(mailAddr, other.mailAddr);
        privilege = other.privilege;

      }
      Data(const char * pass,const char * name_,const char * mail,const int & privilege_) {
        strcpy(password,pass);
        strcpy(name,name_);
        strcpy(mailAddr,mail);
        privilege = privilege_;
      }
      bool operator<(const Data &other) const {
        return strcmp(name,other.name);
      }
      Data & operator=(const Data &other) {
        strcpy(name,other.name);
        strcpy(password,other.password);
        strcpy(mailAddr,other.mailAddr);
        privilege = other.privilege;
        return *this;
      }
    };
    BPlusTree<Data, UserName> users;
    std::map<UserName,int> login_stack;
    const std::string PATH = "../db/user_info.data";
    int total_users = 0;
  public:
    User():users(PATH) {

    }
    bool add_user(const char * ,const char * ,const char *,const char * ,const char *,const int &);
    bool login(const char * username,const char * password);
    bool logout(const char * username);
    bool query_profile(const char * cur_user,const char * username);
    bool modify_profile(const char * cur_user,const char * username,const char* password, const char* name ,const char * mail,int & privilege);
    void buy_ticket();
    void refund_ticket();
    void clean();
  };

  inline bool User::add_user(const char * cur_username,const char * username, const char * password, const char * tangible_name, const char * mail, const int & privilege) {
    UserName cur_user(cur_username);
    auto res = login_stack.find(cur_user);
    if (res == login_stack.end() || res->second <= privilege) {
      return false;
    }
    else{
      UserName new_user_name(username);
      Data new_user_data(password,tangible_name,mail,privilege);
      users.Insert(new_user_name,new_user_data);
    }
  }

  inline bool User::login(const char * username,const char * password) {
    UserName user_name(username);
    auto res = login_stack.find(user_name);
    if (res == login_stack.end() || res->second <= 0) {
      return false;
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name,find);
    if (find) {
      if (strcmp(res_find[0].password,password) == 0) {
        login_stack.insert(std::make_pair(user_name,res_find[0].privilege));
        return true;
      }
    }
    return false;
  }
  inline bool User::logout(const char * username) {
    UserName user_name(username);
    auto res = login_stack.find(user_name);
    if (res == login_stack.end()) {
      return false;
    }
    else {
      login_stack.erase(res);
      return true;
    }
  }
  inline bool User::query_profile(const char * cur_user,const char * username) {
    UserName user_name(username);
    UserName cur_user_name(cur_user);
    auto res = login_stack.find(cur_user_name);
    if (res == login_stack.end()) {
      return false;
    }
    bool find = false;
    sjtu::vector<Data> res_find = users.Search(user_name,find);
    if (find) {
      if (res_find[0].privilege <= res->second) {
        std::cout << username << " " << res_find[0].name<< " " << res_find[0].mailAddr << " " << res_find[0].privilege << "\n";
        return true;
      }
    }
    return false;
  }
  inline bool User::modify_profile(const char *cur_user, const char *username, const char *password, const char *name, const char *mail, int &privilege) {
    UserName user_name(username);
    UserName cur_user_name(cur_user);
    auto res = login_stack.find(cur_user_name);
    if (res == login_stack.end() ) {
      return false;
    }
    if (privilege != -1 && privilege >= res->second) {
      return false;
    }
    else {
      bool find_ = false;
      Data update_user_data(users.Search(user_name,find_)[0]);
      if (update_user_data.privilege <= res->second) {
        if (password != nullptr) {
          strcpy(update_user_data.password,password);
        }
        if (name != nullptr) {
          strcpy(update_user_data.name,name);
        }
        if (mail != nullptr) {
          strcpy(update_user_data.mailAddr,mail);
        }
        if (privilege != -1) {
          update_user_data.privilege = privilege;
        }
        return users.Update(user_name,update_user_data);
      }
      return false;
    }
  }
  inline void User::clean() {

  }



}

#endif //USER_HPP
