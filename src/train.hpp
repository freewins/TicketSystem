//
// Created by Freewings on 2025/4/10.
//

#ifndef STATION_HPP
#define STATION_HPP
#include <iostream>
#include <file.hpp>
#include <vector>
#include "../src/BPlusTree.hpp"
#include "../src/utils.hpp"
#include "../src/vector.hpp"

namespace train {
  class Train {
  private:
    const static int MAX_TRAINS = 30;
    const static int MAX_DAYS = 100;

  private:
    struct TrainId {
      //The key to store the train
      char train_id[50];

      TrainId() {
        train_id[0] = '\0';
      }

      TrainId(const char *train_id_) {
        if (train_id_ != nullptr) {
          strcpy(train_id, train_id_);
        }
      }

      TrainId(const std::string &train_id_) {
        strcpy(train_id, train_id_.c_str());
      }

      bool operator<(const TrainId &t) const {
        return strcmp(train_id, t.train_id) < 0;
      }

      bool operator==(const TrainId &t) const {
        return strcmp(train_id, t.train_id) == 0;
      }

      bool operator!=(const TrainId &t) const {
        return strcmp(train_id, t.train_id) != 0;
      }
    };

    struct StationName {
      char train_name[50];

      StationName() {
        train_name[0] = '\0';
      }

      StationName(const char *train_name_) {
        if (train_name_ != nullptr) {
          strcpy(train_name, train_name_);
        }
      }

      StationName(const std::string &train_name_) {
        if (train_name_.size() > 0) {
          strcpy(train_name, train_name_.c_str());
        }
      }

      StationName(const StationName &t) {
        strcpy(train_name, t.train_name);
      }

      bool operator<(const StationName &t) const {
        return strcmp(train_name, t.train_name) < 0;
      }

      bool operator==(const StationName &t) const {
        return strcmp(train_name, t.train_name) == 0;
      }

      bool operator!=(const StationName &t) const {
        return strcmp(train_name, t.train_name) != 0;
      }
    };

    struct TrainInfoBasic {
      int seat_num = 0;
      bool is_release = false;
      utils::Time start_sale_time, end_sale_time; //出售时间 出售结束时间
      utils::Time endding_time; //最后结束时间
      long long pos;


      TrainInfoBasic() = default;

      TrainInfoBasic(const TrainInfoBasic &t) {
        is_release = t.is_release;
        seat_num = t.seat_num;
        start_sale_time = t.start_sale_time;
        end_sale_time = t.end_sale_time;
        endding_time = t.endding_time;
        pos = t.pos;
      }

      bool operator<(const TrainInfoBasic &t) const {
        return start_sale_time < t.start_sale_time;
      }

      bool operator==(const TrainInfoBasic &t) const {
        return t.pos == pos;
      }

      bool operator!=(const TrainInfoBasic &t) const {
        return t.pos != pos;
      }

      TrainInfoBasic &operator=(const TrainInfoBasic &t) {
        is_release = t.is_release;
        seat_num = t.seat_num;
        start_sale_time = t.start_sale_time;
        end_sale_time = t.end_sale_time;
        endding_time = t.endding_time;
        pos = t.pos;
        return *this;
      }
    };

    struct TrainInfoMost {
      int station_num = 0;
      StationName station_names[MAX_TRAINS] = {};

      int prices[MAX_TRAINS] = {0};
      int arriving_time[MAX_TRAINS] = {0}; // To store the time 前缀和
      int leaving_time[MAX_TRAINS] = {0}; //由于具体的date 是不确定的,所以这时候要存储times的大小
      int max_days = 0;
      //注意此处的值 left_seats[i][j]代表日期标号为 i 时,且当前火车编号为j,从j出发到j + 1站的座位数，0-based
      int left_seats[MAX_DAYS][MAX_TRAINS] = {0};

      utils::Time start_time;
      char type;

      TrainInfoMost() = default;
    };

    struct Info {
      TrainId train_id;
      int time;
      int price;
      int left_seat;
      int date_num;
      utils::Time from, to;

      Info &operator=(const Info &t) {
        train_id = t.train_id;
        time = t.time;
        price = t.price;
        left_seat = t.left_seat;
        from = t.from;
        to = t.to;
        date_num = t.date_num;
        return *this;
      }

      Info() = default;

      Info(const TrainId &train_id_, const utils::Time &date,
           const utils::Time &start_time,
           const int &leaving_time, const int &arriving_time,const int & date_num_): Info(train_id_, date, start_time, 0, 0, leaving_time,
                                                                    arriving_time,date_num_) {
      }

      Info(const TrainId &train_id_, const utils::Time &date, const utils::Time &start_time,
           const int &price_, const int &left_seat_, const int &leaving_time,
           const int &arriving_time,const int & date_num_): from(date, start_time) {
        train_id = train_id_;
        to = from;
        from.add_min(leaving_time);
        to.add_min(arriving_time);
        price = price_;
        left_seat = left_seat_;
        date_num = date_num_;
        time = arriving_time - leaving_time;
      }
    };

    //用于排队队列的信息

    struct queue_key {
      TrainId train_id;
      int time_num; //date转换成对应的num后的值，相当于哈希

      queue_key() = default;

      queue_key(const TrainId &train_id_, const int &time_num_) {
        train_id = train_id_;
        time_num = time_num_;
      }

      bool operator<(const queue_key &t) const {
        if (train_id != t.train_id) {
          return train_id < t.train_id;
        } else {
          return time_num < t.time_num;
        }
      }

      bool operator==(const queue_key &t) const {
        return train_id == t.train_id && time_num == t.time_num;
      }

      bool operator!=(const queue_key &t) const {
        if (train_id == t.train_id) {
          return t.time_num != time_num;
        } else {
          return true;
        }
      }
    };

    struct queue_value {
      char username[25];
      int reversed_seats_num;
      int time_stamp;
      int start_pos, end_pos;

      queue_value() {
        username[0] = '\0';
        reversed_seats_num = 0;
        time_stamp = 0;
        start_pos = 0;
        end_pos = 0;
      }

      queue_value(const char * user_name_,const int &reversed_seats_num_, const int &time_stamp_, const int &start_pos_, const int &end_pos_) {
        strcpy(username, user_name_);
        reversed_seats_num = reversed_seats_num_;
        time_stamp = time_stamp_;
        start_pos = start_pos_;
        end_pos = end_pos_;
      }

      bool operator<(const queue_value &t) const {
        return time_stamp < t.time_stamp;
      }

      bool operator==(const queue_value &t) const {
        return time_stamp == t.time_stamp;
      }

      bool operator!=(const queue_value &t) const {
        return time_stamp != t.time_stamp;
      }
    };


    BPlusTree<TrainInfoBasic, TrainId, 40> trains_basic; //Store the basic train info.
    BPlusTree<TrainId, StationName, 40> stations; //Store the trainID pass the station.
    BPlusTree<queue_value, queue_key, 40> queues;
    file::MyFile<TrainInfoMost> trains_most;
    const std::string PATH_STATIONS = "station_info.bpt";
    const std::string PATH_TRAINS_BASIC = "train_basic_info.bpt";
    const std::string PATH_TRAINS_MOST = "train_stations_info.data";
    const std::string PATH_QUEUE = "queue_info.bpt";
    int total_trains = 0;

  private:
    void get_station_pos(const StationName *p, int station_nums, const StationName &start_station,
                         const StationName &end_station, int &start_pos, int &end_pos) {
      start_pos = -1;
      end_pos = -1;
      for (int i = 0; i < station_nums; i++) {
        if (start_pos < 0 && start_station == p[i]) {
          start_pos = i;
          continue;
        }
        if (end_pos < 0 && end_station == p[i]) {
          end_pos = i;
          continue;
        }
        if (end_pos >= 0 && start_pos >= 0) {
          return;
        }
      }
      return;
    }

  private:
    static bool cmp_price(const Info &t1, const Info &t2) {
      if (t1.price != t2.price) {
        return t1.price < t2.price;
      } else {
        return t1.train_id < t2.train_id;
      }
    }

    static bool cmp_time(const Info &t1, const Info &t2) {
      if (t1.time != t2.time) {
        return t1.time < t2.time;
      } else {
        return t1.train_id < t2.train_id;
      }
    }

    static bool cmp_update(int mode,const long long & tmp_time,const long long & tmp_cost,const long long & ans_time,const long long & ans_cost,const Info & info1, const Info & info2,const TrainId & id1,const TrainId & id2) {
      if (mode) {
        if (tmp_time != ans_time) {
          return tmp_time < ans_time;
        }
        else if (tmp_cost != ans_cost) {
          return tmp_cost < ans_cost;
        }
      }
      else {
        if (tmp_cost != ans_cost) {
          return tmp_cost < ans_cost;
        }
        else if (tmp_time != ans_time) {
          return tmp_time < ans_time;
        }
      }
      if (info1.train_id != id1) {
        return info1.train_id < id1;
      }
      else if (info2.train_id != id2) {
        return info2.train_id < id2;
      }
      else {
        return false;
      }
    }
  public:
    Train(): stations("station_info.bpt"), trains_basic("train_basic_info.bpt"),
             trains_most("train_stations_info.data"),
             queues("queue_info.bpt") {
    }

    //-----Admin-----

    bool AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_,
                  const std::string &stations_,
                  const std::string &prices_, const std::string &
                  startTime_, const std::string &travelTimes_, const std::string &stopoverTimes_,
                  const std::string &saleData_, const char &type_);

    bool ReleaseTrain(const std::string &trainID_);

    bool DeleteTrain(const std::string &trainID_);

    //-----User-----
    //用于QueryTicket 和 QueryTranfer 中的相同的query，返回从一个地方到另一个地方所有的train

    sjtu::vector<TrainId> QueryBasic(StationName fStation, StationName tStation, bool &find);

    /**
     *
     * @param fromStation 出发站
     * @param toStation 终点站
     * @param date 日期
     * @param mode 模式 0 默认为按照票价 1 为按照时间进行排序
     */
    void QueryTicket(const char *fromStation, const char *toStation, utils::Time date, int mode = 0);

    //Query 后自行打印
    bool QueryTrain(const char *TrainID, utils::Time date);

    //QUery 后自行打印
    void QueryTransfer(const char *fromStation, const char *toStation, utils::Time date, int mode = 0);

    //这两个函数必须被user的函数调用，也就是说user中必须存在对应的函数
    /**
     *
     * @param usename 用户名
     * @param trainID 列车ID
     * @param startStation 始发站
     * @param endStation 终点站
     * @param date 日期
     * @param queue 是否接受队列等待
     * @return 返回结果 : -1-说明购票失败 0-说明进入队列等候，1 说明购票成功
     */
    int BuyTicket(const char *username_, const char *trainID_, const char *startStation_, const char *endStation_,
                  utils::Time date_, int num, bool queue_, int timestamp, utils::Time &leaving_time,
                  utils::Time &arriving_time, int &price, int &per_price, int &date_num);

    /**
     *
     * @param username_
     * @param trainID_
     * @param startStation_
     * @param endStation_
     * @param date_
     * @param timestamp
     * @param num
     * @param queue_
     * @return
     */
    bool RefundTicket(const char *username_, const char *trainID_, const char *startStation_, const char *endStation_,
                      int timenum, int timestamp, int num, bool queue_,
                      sjtu::vector<utils::transfer_union> &change);

    bool clean();
  };


  inline bool Train::AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_,
                              const std::string &stations_, const std::string &prices_, const std::string &startTime_,
                              const std::string &travelTimes_, const std::string &stopoverTimes_,
                              const std::string &saleData_, const char &type_) {
    TrainId trainID(trainID_);
    bool repeat = false;
    trains_basic.Search(trainID, repeat);
    if (repeat) {
      return false;
    }
    TrainInfoBasic train_info_basic;
    TrainInfoMost train_info_most;
    train_info_basic.seat_num = seatNum_;
    train_info_basic.is_release = false; {
      int pos = saleData_.find('|', 0);
      if (pos != std::string::npos) {
        train_info_basic.start_sale_time = utils::string_to_time(saleData_.substr(0, pos), 2);
        train_info_basic.end_sale_time = utils::string_to_time(saleData_.substr(pos + 1), 2);

        if (train_info_basic.end_sale_time.cmp < train_info_basic.start_sale_time.cmp) {
          return false;
        }
      } else {
        return false;
      }
    }
    long long pos = trains_most.getPos();
    train_info_basic.pos = pos;
    train_info_most.station_num = stationNum_;
    train_info_most.type = type_;
    train_info_most.start_time = utils::string_to_time(startTime_, 1);
    int *travel = new int[stationNum_];
    int *stopover = new int[stationNum_];
    //路程时间
    utils::split_int(travel, travelTimes_, '|');
    //经停时间
    if (stationNum_ > 2) {
      utils::split_int(stopover, stopoverTimes_, '|');
    }
    train_info_most.arriving_time[0] = 0;
    train_info_most.leaving_time[0] = 0;
    for (int i = 1; i < stationNum_; i++) {
      train_info_most.arriving_time[i] += train_info_most.leaving_time[i - 1] + travel[i - 1];
      if (stationNum_ > 2 && i >= 1 && i < stationNum_ - 1) {
        train_info_most.leaving_time[i] = train_info_most.arriving_time[i];
        train_info_most.leaving_time[i] += stopover[i - 1];
      }
    }
    //这里要进行 + 1,因为得到是间隔而非总数
    train_info_most.max_days = train_info_basic.start_sale_time.minus_mon(train_info_basic.end_sale_time,
                                                                     train_info_basic.start_sale_time) + 1;
    train_info_basic.end_sale_time.reset_hour(train_info_most.start_time);
    train_info_basic.endding_time = train_info_basic.end_sale_time;
    train_info_basic.endding_time = train_info_basic.endding_time.add_min(train_info_most.arriving_time[stationNum_ - 1]);
    //处理车站
    sjtu::vector<std::string> stations_vector = utils::split(stations_, '|');
    for (int i = 0; i < train_info_most.station_num ; i++) {
      train_info_most.station_names[i] = stations_vector[i];
    }
    //车票价格
    train_info_most.prices[0] = 0;
    utils::split_int(train_info_most.prices + 1, prices_, '|');
    for (int i = 1; i <= stationNum_ - 1; i++) {
      train_info_most.prices[i] += train_info_most.prices[i - 1];
    }

    for (int i = 0; i < stationNum_; i++) {
      train_info_most.left_seats[0][i] = seatNum_;
    }
    for (int i = 1; i < train_info_most.max_days; i++) {
      memcpy(train_info_most.left_seats[i], train_info_most.left_seats[0], sizeof(int) * (stationNum_ + 1));
    }
    //TODO 可以进行时间上的优化

    trains_most.write(train_info_most);
    for (int i = 0; i < stations_.size(); ++i) {
      stations.Insert(train_info_most.station_names[i], trainID);
    }
    delete [] travel;
    delete [] stopover;
    if (trains_basic.Insert(trainID, train_info_basic)) {
      return true;
    } else {
      return false;
    }
  }


  inline bool Train::ReleaseTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    auto res = trains_basic.Search(trainID, find);

    if (find) {
      TrainInfoBasic info_basic(res[0]); //如果没有找到 -> optional
      if (info_basic.is_release == false) {
        info_basic.is_release = true;
        trains_basic.Update(trainID, info_basic);
        return true;
      } else {
        return false;
      }
    }
    return false;
  }


  inline bool Train::DeleteTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    TrainInfoBasic info_basic(trains_basic.Search(trainID, find)[0]);
    if (find) {
      if (!info_basic.is_release) {
        TrainInfoMost train_info_most;
        trains_most.read(info_basic.pos, train_info_most);
        for (int j = 0; j < train_info_most.station_num; j++) {
          stations.Remove(train_info_most.station_names[j], trainID);
        }
        trains_basic.Remove(trainID, info_basic);
        return true;
      }
    }
    return false;
  }

  inline sjtu::vector<Train::TrainId> Train::QueryBasic(StationName fStation, StationName tStation, bool &find) {
    sjtu::vector<Train::TrainId> res;
    sjtu::vector<Train::TrainId> fTrains;
    sjtu::vector<Train::TrainId> tTrains;

    find = false;
    fTrains = stations.Search(fStation, find);
    if (find) {
      tTrains = stations.Search(tStation, find);
      if (find) {
        int i = 0, j = 0;
        while (i < fTrains.size() && j < tTrains.size()) {
          if (fTrains[i] == tTrains[j]) {
            res.push_back(fTrains[i]);
            i++;
            j++;
          } else if (fTrains[i] < tTrains[j]) {
            i++;
          } else {
            j++;
          }
        }
      }
    }
    return res;
  }

  inline bool Train::QueryTrain(const char *TrainID, utils::Time date) {
    TrainId trainID(TrainID);
    bool find = false;
    auto basic_info = trains_basic.Search(trainID, find);
    if (find) {
      if (basic_info[0].end_sale_time.cmp >= date.cmp && basic_info[0].start_sale_time.cmp <= date.cmp) {
        TrainInfoMost train_info_most;
        trains_most.read(basic_info[0].pos, train_info_most);
        int date_num = date.minus_mon(basic_info[0].start_sale_time, date);
        date.reset_hour(train_info_most.start_time);
        std::cout << trainID.train_id << " " << train_info_most.type << std::endl;
        for (int i = 0; i < train_info_most.station_num; i++) {
          std::cout << train_info_most.station_names[i].train_name << " ";
          if (i == 0) {
            std::cout << "xx-xx xx:xx";
          } else {
            //TODO 这里是否要优化
            std::cout << date.add_min(train_info_most.arriving_time[i]);
          }
          std::cout << " -> ";
          if (i == train_info_most.station_num - 1) {
            std::cout << "xx-xx xx:xx ";
            std::cout << train_info_most.prices[i] << " x\n";
          } else {
            std::cout << date.add_min(train_info_most.leaving_time[i]) << " ";
            std::cout << train_info_most.prices[i] << " " << train_info_most.left_seats[date_num][i] << "\n";
          }
        }
        return true;
      } else {
        return false;
      }
    }
    return false;
  }


  inline void Train::QueryTicket(const char *fromStation, const char *toStation, utils::Time date, int mode) {
    StationName fStation(fromStation);
    StationName tStation(toStation);
    bool find = false;
    sjtu::vector<Train::TrainId> path;//
    sjtu::vector<int> pos_traind_id; //映射  pos_vecotr[k] -> item -> trainID -> path[pos_train_id[k]]
    sjtu::vector<TrainInfoBasic> basic_data;
    path = this->QueryBasic(fStation, tStation, find);
    if (find) {
      for (int i = 0; i < path.size(); i++) {
        bool find_ = false;
        TrainInfoBasic info(trains_basic.Search(path[i], find_)[0]);
        //没有release的车次不能被查询
        if (!info.is_release)continue;
        if (info.start_sale_time.cmp <= date.cmp && info.endding_time.cmp >= date.cmp) {
          pos_traind_id.push_back(i);
          basic_data.push_back(info);
        }
      }
      TrainInfoMost train_info_most;

      sjtu::vector<Info> infos;
      for (int i = 0; i < basic_data.size(); i++) {
        trains_most.read(basic_data[i].pos, train_info_most);
        int start_pos, end_pos;
        get_station_pos(train_info_most.station_names, train_info_most.station_num, fStation, tStation, start_pos,
                        end_pos);
        if (end_pos < start_pos) {
          continue;
        }
        int date_num = date.minus_mon(date,basic_data[i].start_sale_time);
        Info new_info(path[pos_traind_id[i]], date, train_info_most.start_time, train_info_most.leaving_time[start_pos],
                      train_info_most.arriving_time[end_pos],0); //这里用0是因为无法确定具体的date_num
        utils::getDateNum(date, train_info_most.start_time, new_info.from, new_info.to,
                          train_info_most.leaving_time[start_pos], train_info_most.arriving_time[end_pos], date_num);
        if (date_num < 0 || new_info.from > basic_data[i].end_sale_time.add_min(train_info_most.leaving_time[start_pos])) {
          //第二个判断条件是为了避免出现购买时间靠后的情况 具体见test-basic_6-24-1748455609.log
          continue;
        }
        new_info.date_num = date_num;
        new_info.time = train_info_most.arriving_time[end_pos] - train_info_most.leaving_time[start_pos];
        new_info.price = train_info_most.prices[end_pos] - train_info_most.prices[start_pos];
        new_info.left_seat = 1e5 + 10; //设置成最大值
        for (int i = start_pos; i < end_pos; i++) {
          new_info.left_seat = std::min(new_info.left_seat, train_info_most.left_seats[date_num][i]);
        }
        infos.push_back(new_info);
      }

      if (mode) {
        std::sort(infos.begin(), infos.end(), cmp_time);
      } else {
        std::sort(infos.begin(), infos.end(), cmp_price);
      }
      std::cout << infos.size() << "\n";
      for (int k = 0; k < infos.size(); k++) {
        std::cout << infos[k].train_id.train_id << " " << fStation.train_name << " " << infos[k].from << " -> " << tStation.train_name <<
            " " << infos[k].to <<
            " " << infos[k].price << " " << infos[k].left_seat << "\n";
      }
    } else {
      std::cout << "0\n";
    }
    return;
  }

  inline void Train::QueryTransfer(const char *fromStation, const char *toStation, utils::Time date, int mode) {
    StationName fStation(fromStation);
    StationName tStation(toStation);
    //Step 1: 获得所有的 从fStation出发的点 ，然后获得在date内能到达的Stations
    //Step 2: 对于这些Station，再求从这些Station 能否到达to Station 的trainID
    //Step 3: 如果可以到达，注意到达的时间，首先看leaving_time 如果leaving time并不满足，就增加一天 这个也要注意发行时间
    bool find = false;
    sjtu::vector<TrainId> trainIDs = stations.Search(fStation, find);
    sjtu::vector<int> hash; //建立一个从pos -> train ID 的hash
    sjtu::vector<utils::Time> basic_sale_date;
    sjtu::vector<utils::Time> end_sale_date;
    sjtu::vector<long long> pos_vector;
    if (find) {
      for (int i = 0; i < trainIDs.size(); i++) {
        bool find_ = false;
        auto trains_res = trains_basic.Search(trainIDs[i], find_);
        if (find_) {
          //TODO 改成最后的运营结束时间
          if (!trains_res[0].is_release)continue; //未release 不能被查询
          if (trains_res[0].start_sale_time.cmp <= date.cmp && trains_res[0].endding_time.cmp >= date.cmp) {
            pos_vector.push_back(trains_res[0].pos);
            basic_sale_date.push_back(trains_res[0].start_sale_time);
            end_sale_date.push_back(trains_res[0].end_sale_time);

            hash.push_back(i);
          }
        }
      }
      if (!pos_vector.empty()) {
        TrainInfoMost train_info_most;
        sjtu::vector<StationName> midStations;
        sjtu::vector<Info> one_infos;
        bool add_ = false;
        int fpos = 0;
        for (int i = 0; i < pos_vector.size(); i++) {
          trains_most.read(pos_vector[i], train_info_most);
          add_ = false;
          Info first_info;
          first_info.train_id = trainIDs[hash[i]];
          first_info.from = date;
          first_info.from.reset_hour(train_info_most.start_time);
          if (train_info_most.station_names[train_info_most.station_num - 1] == fStation) {
            continue;
          }
          for (int k = 0; k < train_info_most.station_num; k++) {
            if (!add_ && train_info_most.station_names[k] == fStation) {
              add_ = true;
              int date_num = date.minus_mon(basic_sale_date[i], date);
              first_info.from = first_info.from.add_min(train_info_most.leaving_time[k]);
              int d = date.minus_mon(first_info.from, date); //用来计算对应始发车对应的出发日期
              if (d > 0) {
                date_num -= d;
                if (date_num < 0) {
                  break;
                }
                first_info.from.minus_day(d);
              }
              if (first_info.from > end_sale_date[i].add_min(train_info_most.leaving_time[k])) {
                break;
              }
              fpos = k;
              first_info.date_num = date_num;
              first_info.left_seat = train_info_most.left_seats[first_info.date_num][fpos];
              continue;
            }
            if (add_) {
              midStations.push_back(train_info_most.station_names[k]);
              first_info.to = first_info.from.add_min(train_info_most.arriving_time[k] - train_info_most.leaving_time[fpos] );
              first_info.price = train_info_most.prices[k] - train_info_most.prices[fpos];
              first_info.time = train_info_most.arriving_time[k] - train_info_most.arriving_time[fpos];
              first_info.left_seat = std::min(first_info.left_seat,train_info_most.left_seats[first_info.date_num][k - 1]);
              one_infos.push_back(first_info);
            }
          }
        }
        //step 3:
        if (!midStations.empty()) {
          sjtu::vector<TrainId> to_train_ids; //存储第二趟车的trainID
          Info info2,info_tmp;
          int first_chosen = 0; //标记符合transfer再infos中的位置
          StationName mid; //中转站
          const long long INF = 0x3f3f3f3f3f;
          long long ans_time = INF,ans_cost = INF;
          for (int i = 0; i < midStations.size(); i++) {
            //std::cout << midStations[i].train_name << "\n";
            bool find_ = false;
            auto mid_trains = QueryBasic(midStations[i], tStation, find_);
            //auto mid_train_ids = trainIDs[hash[train_ids[i]]]; //第一班车的trainID
            if (find_) {
              for (int j = 0; j < mid_trains.size(); j++) {
                if (mid_trains[j] == one_infos[i].train_id) {
                  //说明是统一趟车
                  continue;
                } else
                  {
                  bool find_to_train = false;
                  auto res = trains_basic.Search(mid_trains[j], find_to_train);
                  //未release 不能被查询
                  if (!res[0].is_release)continue;
                  // 大于上一班车的到达时间
                  if (res[0].endding_time.cmp >= date.cmp && res[0].endding_time.cmp >=one_infos[i].to.cmp) {
                    if (res[0].start_sale_time < one_infos[i].to) {
                      info_tmp.from = one_infos[i].to;
                      info_tmp.from.reset_hour({0,0,0,0});
                    }
                    else {
                      info_tmp.from = res[0].start_sale_time;
                    }
                    TrainInfoMost train_info_most_;
                    trains_most.read(res[0].pos, train_info_most_);
                    int start_pos, end_pos;
                    get_station_pos(train_info_most_.station_names, train_info_most_.station_num, midStations[i],
                                    tStation, start_pos, end_pos);
                    if (start_pos <= end_pos) {
                      int date_num = date.minus_mon(info_tmp.from,res[0].start_sale_time);
                      utils::getDateNum(info_tmp.from,train_info_most_.start_time,info_tmp.from,info_tmp.to,
                        train_info_most_.leaving_time[start_pos],train_info_most_.arriving_time[end_pos],date_num);
                      if (date_num < 0) {
                        info_tmp.from = utils::Time(res[0].start_sale_time,train_info_most_.start_time);
                        info_tmp.to = info_tmp.from;
                        date_num = 0;
                        info_tmp.from = info_tmp.from.add_min(train_info_most_.leaving_time[start_pos]);
                        info_tmp.to = info_tmp.to.add_min(train_info_most_.arriving_time[end_pos]);
                      }
                      if (info_tmp.from < one_infos[i].to) {
                        info_tmp.from.add_day(1);
                        info_tmp.to.add_day(1);
                        date_num++;
                      }
                      if (info_tmp.from > res[0].end_sale_time.add_min(train_info_most_.leaving_time[start_pos])) {
                        continue;
                      }
                      info_tmp.price = train_info_most_.prices[end_pos] - train_info_most_.prices[start_pos];
                      bool update_min = false;
                      int tmp_time = info_tmp.to - one_infos[i].from;
                      int tmp_cost = one_infos[i].price + info_tmp.price;
                      update_min = cmp_update(mode,tmp_time,tmp_cost,ans_time,ans_cost,one_infos[first_chosen],info2,one_infos[i].train_id,mid_trains[j]);
                      if (update_min) {
                        ans_cost = tmp_cost;
                        ans_time = tmp_time;
                        mid = midStations[i];
                        first_chosen = i;
                        info_tmp.train_id = mid_trains[j];
                        info_tmp.left_seat = res[0].seat_num + 1;
                        for (int i = start_pos; i < end_pos; i++) {
                          info_tmp.left_seat = std::min(info_tmp.left_seat, train_info_most_.left_seats[date_num][i]);
                        }
                        info2 = info_tmp;
                      }
                    }
                  }
                }
              }
            }
          }
          if (ans_cost != INF && ans_time != INF) {
            std::cout << one_infos[first_chosen].train_id.train_id << " " << fromStation << " " << one_infos[first_chosen].from << " -> " << mid.train_name
                <<
                " " << one_infos[first_chosen].to <<
                " " << one_infos[first_chosen].price << " " << one_infos[first_chosen].left_seat << "\n";
            std::cout << info2.train_id.train_id << " " << mid.train_name << " " << info2.from << " -> " << toStation <<
                " " << info2.to <<
                " " << info2.price << " " << info2.left_seat << "\n";
            return;
          }
        }
      }
    }
    std::cout << "0\n";
  }

  inline int Train::BuyTicket(const char *username_, const char *trainID_, const char *startStation_,
                              const char *endStation_,
                              utils::Time date_, int num, bool queue_, int timestamp, utils::Time &leaving_time,
                              utils::Time &arriving_time, int &price, int &per_price, int &date_num) {
    TrainId trainID(trainID_);
    StationName start_station(startStation_);
    StationName end_station(endStation_);
    TrainInfoBasic basic;
    bool find = false;
    auto res = trains_basic.Search(trainID, find);
    if (find) {
      basic = res[0];
      if (basic.is_release && num <= basic.seat_num && basic.start_sale_time.cmp <= date_.cmp && date_.cmp <= basic.endding_time.
          cmp) {
        TrainInfoMost train_info_most;
        trains_most.read(basic.pos, train_info_most);
        int pos_start_station = -1, pos_end_station = -1;
        int permit_nums = basic.seat_num + 1;
        this->get_station_pos(train_info_most.station_names, train_info_most.station_num, start_station, end_station,
                              pos_start_station, pos_end_station);
        if (pos_start_station >= 0 && pos_end_station >= 0 && pos_start_station < pos_end_station) {
          int num_date = date_.minus_mon(basic.start_sale_time,date_);
          utils::getDateNum(date_, train_info_most.start_time, leaving_time, arriving_time,
                            train_info_most.leaving_time[pos_start_station],
                            train_info_most.arriving_time[pos_end_station], num_date);
          if (num_date < 0 || leaving_time > basic.end_sale_time.add_min(train_info_most.leaving_time[pos_start_station])) {
            return -1;
          }
          date_num = num_date;
          for (int i = pos_start_station; i < pos_end_station; i++) {
            permit_nums = std::min(permit_nums, train_info_most.left_seats[num_date][i]);
          }
          per_price = train_info_most.prices[pos_end_station] - train_info_most.prices[pos_start_station];
          if (permit_nums < num) {
            if (queue_) {
              queue_key key_q(trainID, num_date);
              queue_value value_q(username_,num, timestamp, pos_start_station, pos_end_station);
              bool res = queues.Insert(key_q, value_q);
              if (res) {
                return 0;
              } else {
                return -1;
              }
            } else {
              return -1;
            }
          } else {
            for (int i = pos_start_station; i < pos_end_station; i++) {
              train_info_most.left_seats[num_date][i] -= num;
            }
            price = per_price * num;
            trains_most.update(basic.pos, train_info_most);
            return 1;
          }
        } else {
          return -1;
        }
      }
    }
    return -1;
  }

  inline bool Train::RefundTicket(const char *username_, const char *trainID_, const char *startStation_,
                                  const char *endStation_, int timenum, int timestamp, int num,
                                  bool queue_, sjtu::vector<utils::transfer_union> &change) {
    TrainId trainID(trainID_);
    StationName start_station(startStation_);
    StationName end_station(endStation_);
    bool find = false;
    auto res_tmp = trains_basic.Search(trainID, find);
    if (find) {
      TrainInfoBasic info_basic = res_tmp[0];
      if (queue_) {
        //在队列中
        queues.Remove({trainID, timenum}, {"",num, timestamp, 0, 0});
      } else {
        //不在队列，进行更新数据，检测是否有新的可以订票的内容
        TrainInfoMost train_info_most;
        trains_most.read(info_basic.pos, train_info_most);
        int pos_start_station = -1, pos_end_station = -1;
        this->get_station_pos(train_info_most.station_names, train_info_most.station_num, start_station, end_station,
                              pos_start_station, pos_end_station);
        if (pos_start_station < 0 || pos_end_station < 0) {
          return false;
        }
        if (pos_start_station > pos_end_station) {
          return false;
        }
        for (int i = pos_start_station; i < pos_end_station; i++) {
          train_info_most.left_seats[timenum][i] += num;
        }

        queue_key key_q(trainID, timenum);
        sjtu::vector<queue_value> res = queues.Search(key_q, find);
        if (find) {
          //确定有队列的等待
          for (int s = 0; s < res.size(); s++) {
            bool change_ = true;
            for (int i = res[s].start_pos; i < res[s].end_pos; i++) {
              if (train_info_most.left_seats[timenum][i] < res[s].reversed_seats_num) {
                change_ = false;
                break;
              }
            }
            if (change_) {
              //说明可以进行修改
              queues.Remove(key_q, res[s]);
              for (int i = res[s].start_pos; i < res[s].end_pos; i++) {
                train_info_most.left_seats[timenum][i] -= res[s].reversed_seats_num;
              }
              change.push_back(utils::transfer_union(res[s].username, res[s].time_stamp, 1));
            }
          }
        }
        trains_most.update(info_basic.pos, train_info_most);
      }
      return true;
    } else {
      return false;
    }
  }

  inline bool Train::clean() {
    std::filesystem::path f_path_stations = PATH_STATIONS;
    std::filesystem::path f_path_trains_basic = PATH_TRAINS_BASIC;
    std::filesystem::path f_path_trains_most = PATH_TRAINS_MOST;
    std::filesystem::path path_queue = PATH_QUEUE;
    try {
      if (std::filesystem::remove(f_path_stations) && std::filesystem::exists(f_path_trains_basic) &&
          std::filesystem::exists(f_path_trains_most) && std::filesystem::exists(path_queue)) {
        return true;
      } else {
        return false;
      }
    } catch (std::filesystem::filesystem_error &e) {
      std::cout << e.what() << std::endl;
      return false;
    }
  }
}

#endif //STATION_HPP
