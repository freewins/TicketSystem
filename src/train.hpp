//
// Created by Freewings on 2025/4/10.
//

#ifndef STATION_HPP
#define STATION_HPP
#include <iostream>
#include <file.hpp>

#include "../src/BPlusTree.hpp"
#include "../src/utils.hpp"
#include "../src/vector.hpp"

namespace train {
  //TODO Add the constructor


  class Train {
  private:
    const static int MAX_TRAINS = 26;
    const static int MAX_DAYS = 100;

  private:
    struct TrainId {
      //The key to store the train
      char train_id[50];

      TrainId() {
        strcpy(train_id, "");
      }

      TrainId(const char *train_id_) {
        strcpy(train_id, train_id_);
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
        strcpy(train_name, "");
      }

      StationName(const char *train_name_) {
        strcpy(train_name, train_name_);
      }

      StationName(const std::string &train_name_) {
        strcpy(train_name, train_name_.c_str());
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
      utils::Time start_time, end_time; //出售时间 出售结束时间
      long long pos;


      TrainInfoBasic() = default;

      TrainInfoBasic(const TrainInfoBasic &t) {
        is_release = t.is_release;
        seat_num = t.seat_num;
        start_time = t.start_time;
        end_time = t.end_time;
      }

      bool operator<(const TrainInfoBasic &t) const {
        return start_time < t.start_time;
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
        start_time = t.start_time;
        end_time = t.end_time;
        pos = t.pos;
        return *this;
      }
    };

    struct TrainInfoMost {
      int station_num = 0;
      StationName station_names[MAX_TRAINS];

      int prices[MAX_TRAINS]{0};
      int times[MAX_TRAINS]{0}; // To store the time 前缀和
      utils::Time arriving_time[MAX_TRAINS];
      utils::Time leaving_time[MAX_TRAINS];
      int max_days = 0;
      //注意此处的值 left_seats[i][j]代表日期标号为 i 时,且当前火车编号为j,从j出发到j + 1站的座位数，0-based
      int left_seats[MAX_DAYS][MAX_TRAINS]{0};

      utils::Time start_time;
      char type;

      TrainInfoMost() = default;
    };

    struct Info {
      TrainId train_id;
      int time;
      int price;
      int left_seat;
      utils::Time from, to;
    };

    //用于排队队列的信息

    struct queue_key {
      TrainId train_id;
      int time_num; //date转换成对应的num后的值，相当于哈希

      queue_key() = default;

      queue_key(const TrainId &train_id_, const int time_num_) {
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
        return train_id == t.train_id;
      }

      bool operator!=(const queue_key &t) const {
        return train_id != t.train_id;
      }
    };

    struct queue_value {
      int reversed_seats_num;
      int time_stamp;
      int start_pos, end_pos;

      queue_value() = default;

      queue_value(const int &reversed_seats_num_, const int time_stamp_, const int start_pos_, const int end_pos_) {
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
    const std::string PATH_STATIONS = "../db/station_info.bpt";
    const std::string PATH_TRAINS_BASIC = "../db/train_basic_info.bpt";
    const std::string PATH_TRAINS_MOST = "../db/train_stations_info.data";
    const std::string PATH_QUEUE = "../db/queue_info.bpt";
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

  public:
    //TODO add a total information file
    Train(): stations(PATH_STATIONS), trains_basic(PATH_TRAINS_BASIC), trains_most(PATH_TRAINS_MOST),
             queues(PATH_QUEUE) {
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
    bool QueryTrain(const char *TrainID, const utils::Time &date);

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
                  const utils::Time &date_, int num, bool queue_, int timestamp, utils::Time &leaving_time,
                  utils::Time &arriving_time, int &price);

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
                      const utils::Time &date_, int timestamp, int num, bool queue_,
                      sjtu::vector<utils::transfer_union> &change);

    bool clean();
  };

  //TODO
  inline bool Train::AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_,
                              const std::string &stations_, const std::string &prices_, const std::string &startTime_,
                              const std::string &travelTimes_, const std::string &stopoverTimes_,
                              const std::string &saleData_, const char &type_) {
    TrainId trainID(trainID_);
    TrainInfoBasic train_info_basic;
    TrainInfoMost train_info_most;
    train_info_basic.seat_num = seatNum_;
    train_info_basic.is_release = false; {
      int pos = saleData_.find('|', 0);
      if (pos != std::string::npos) {
        train_info_basic.start_time = utils::string_to_time(saleData_.substr(0, pos), 2);
        train_info_basic.end_time = utils::string_to_time(saleData_.substr(pos + 1), 2);
        if (train_info_basic.end_time.cmp < train_info_basic.start_time.cmp) {
          return false;
        }
      } else {
        return false;
      }
    }
    long long pos = trains_most.getPos();
    train_info_basic.pos = pos;
    if (trains_basic.Insert(trainID, train_info_basic)) {
      train_info_most.station_num = stationNum_;
      train_info_most.type = type_;
      train_info_most.start_time = utils::string_to_time(startTime_, 1);
      //处理车站
      std::vector<std::string> stations_vector = utils::split(stations_, '|');
      for (int i = 0; i < stations_vector.size(); i++) {
        train_info_most.station_names[i] = stations_vector[i];
      }
      //车票价格
      train_info_most.prices[0] = 0;
      utils::split_int(train_info_most.prices, prices_, '|');
      for (int i = 1; i <= stationNum_; i++) {
        train_info_most.prices[i] += train_info_most.prices[i - 1];
      }
      int *travel = new int[stationNum_];
      int *stopover = new int[stationNum_];
      //路程时间
      utils::split_int(travel, travelTimes_, '|');
      //经停时间
      if (stationNum_ > 2) {
        utils::split_int(stopover, stopoverTimes_, '|');
      }
      train_info_most.arriving_time[0] = utils::Time();
      train_info_most.leaving_time[0] = train_info_most.start_time;
      train_info_most.times[0] = 0;
      for (int i = 1; i < stationNum_; i++) {
        train_info_most.times[i] += train_info_most.times[i - 1] + travel[i - 1];
        if (stationNum_ > 2 && i > 1) {
          train_info_most.times[i] += stopover[i - 2];
        }
        train_info_most.arriving_time[i] = train_info_most.arriving_time[i - 1].add_min(train_info_most.times[i]);
        if (stationNum_ > 2 && i >= 1 && i < stationNum_ - 1) {
          train_info_most.leaving_time[i] = train_info_most.arriving_time[i];
          train_info_most.leaving_time[i].add_min(stopover[i - 1]);
        }
      }
      //这里要进行 + 1,因为得到是间隔而非总数
      train_info_most.max_days = train_info_basic.start_time.minus_mon(train_info_basic.end_time,
                                                                       train_info_basic.start_time) + 1;
      for (int i = 0; i < stationNum_; i++) {
        train_info_most.left_seats[0][i] = seatNum_;
      }
      for (int i = 1; i < train_info_most.max_days; i++) {
        memcpy(train_info_most.left_seats[i], train_info_most.left_seats[0], sizeof(int) * (stationNum_ + 1));
      }
      trains_most.write(train_info_most);
      for (int i = 0; i < stations_.size(); ++i) {
        stations.Insert(train_info_most.station_names[i], trainID);
      }
      return true;
    } else {
      return false;
    }
  }


  inline bool Train::ReleaseTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    TrainInfoBasic info_basic(trains_basic.Search(trainID, find)[0]); //如果没有找到 -> optional
    if (find) {
      if (info_basic.is_release == false) {
        info_basic.is_release = true;
        trains_basic.Update(trainID, info_basic);
      }
      return true;
    }
    return false;
  }

  //TODO
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
          } else if (fTrains[j] < tTrains[i]) {
            j++;
          } else {
            i++;
          }
        }
      }
    }
    return res;
  }

  inline bool Train::QueryTrain(const char *TrainID, const utils::Time &date) {
    TrainId trainID(TrainID);
    bool find = false;
    auto basic_info = trains_basic.Search(trainID, find);
    if (find) {
      if (basic_info[0].end_time.cmp >= date.cmp && basic_info[0].start_time.cmp <= date.cmp) {
        TrainInfoMost train_info_most;
        trains_most.read(basic_info[0].pos, train_info_most);
        int date_num = date.minus_mon(basic_info[0].start_time, date);
        std::cout << trainID.train_id << " " << train_info_most.type << std::endl;
        for (int i = 0; i < train_info_most.station_num; i++) {
          std::cout << train_info_most.station_names[i].train_name << " ";
          if (i == 0) {
            std::cout << "xx-xx xx:xx";
          } else {
            std::cout << train_info_most.arriving_time[i];
          }
          std::cout << " -> ";
          if (i == train_info_most.station_num - 1) {
            std::cout << "xx-xx xx:xx ";
            std::cout << train_info_most.prices[i] << " x\n";
          } else {
            std::cout << train_info_most.leaving_time[i] << " ";
            std::cout << train_info_most.prices[i] << " " << train_info_most.left_seats[date_num][i] << "\n";
          }
        }
      }
      return true;
    }
    return false;
  }


  inline void Train::QueryTicket(const char *fromStation, const char *toStation, utils::Time date, int mode) {
    StationName fStation(fromStation);
    StationName tStation(toStation);
    bool find = false;
    sjtu::vector<Train::TrainId> path;
    sjtu::vector<long long> pos_vector;
    path = this->QueryBasic(fStation, tStation, find);
    if (find) {
      for (int i = 0; i < path.size(); i++) {
        bool find_ = false;
        TrainInfoBasic info(trains_basic.Search(path[i], find_)[0]);
        if (info.start_time.cmp <= date.cmp && info.end_time.cmp >= date.cmp) {
          pos_vector.push_back(info.pos);
        }
      }
      TrainInfoMost train_info_most;

      sjtu::vector<Info> infos;
      for (int i = 0; i < pos_vector.size(); i++) {
        trains_most.read(pos_vector[i], train_info_most);
        int start_pos, end_pos;
        get_station_pos(train_info_most.station_names, train_info_most.station_num, fStation, tStation, start_pos,
                        end_pos);
        int date_num = date.minus_mon(train_info_most.start_time, date);
        Info new_info;
        new_info.from = train_info_most.leaving_time[start_pos];
        new_info.to = train_info_most.arriving_time[end_pos];
        new_info.time = train_info_most.prices[end_pos] - train_info_most.prices[start_pos];
        new_info.price = train_info_most.prices[end_pos] - train_info_most.prices[start_pos];
        new_info.train_id = path[i];
        infos.push_back(new_info);
        //TODO 添加一个获取可以预定座位的函数
        new_info.left_seat = 1e5 + 10; //设置成最大值
        for (int i = start_pos; i < end_pos; i++) {
          new_info.left_seat = std::min(new_info.left_seat, train_info_most.left_seats[date_num][i]);
        }
      }

      if (mode) {
        std::sort(infos.begin(), infos.end(), cmp_time);
      } else {
        std::sort(infos.begin(), infos.end(), cmp_price);
      }
      std::cout << infos.size() << "\n";
      for (int k = 0; k < infos.size(); k++) {
        std::cout << infos[k].train_id.train_id << " " << fromStation << " " << infos[k].from << " -> " << toStation <<
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
    sjtu::vector<long long> pos_vector;
    if (find) {
      for (int i = 0; i < trainIDs.size(); i++) {
        bool find_ = false;
        auto trains_res = trains_basic.Search(trainIDs[i], find_);
        if (find_) {
          //TODO 改成最后的运营结束时间
          if (trains_res[0].start_time.cmp <= date.cmp && trains_res[0].end_time.cmp >= date.cmp) {
            pos_vector.push_back(trains_res[0].pos);
            hash.push_back(i);
          }
        }
      }
      if (!pos_vector.empty()) {
        TrainInfoMost train_info_most;
        sjtu::vector<StationName> midStations;
        sjtu::vector<int> train_ids; // TO get the train id -> trainID[hash[train_ids[i]]]]
        sjtu::vector<int> to_value;
        bool add_ = false;
        int fpos = 0;
        for (int i = 0; i < pos_vector.size(); i++) {
          trains_most.read(pos_vector[i], train_info_most);
          add_ = false;

          if (train_info_most.station_names[train_info_most.station_num - 1] == fStation) {
            continue;
          }
          for (int k = 0; k < train_info_most.station_num; k++) {
            if (train_info_most.station_names[k] == fStation) {
              add_ = true;
              fpos = k;
              continue;
            }
            if (add_) {
              midStations.push_back(train_info_most.station_names[k]);
              train_ids.push_back(i);
              if (mode == 0)to_value.push_back(train_info_most.prices[k] - train_info_most.prices[fpos]); //
              else to_value.push_back(train_info_most.times[k] - train_info_most.times[fpos]); //
            }
          }
        }
        //step 3:
        if (!midStations.empty()) {
          sjtu::vector<int> to_values;
          sjtu::vector<TrainId> to_train_ids; //存储第二趟车的trainID
          Info info1, info2;
          StationName mid; //中转站
          const long long INF = 0x3f3f3f3f3f;
          long long ans = INF;
          for (int i = 0; i < midStations.size(); i++) {
            bool find_ = false;
            auto mid_trains = QueryBasic(midStations[i], tStation, find);
            auto mid_train_ids = trainIDs[hash[train_ids[i]]]; //第一班车的trainID
            if (find) {
              for (int j = 0; j < mid_trains.size(); j++) {
                if (mid_trains[j] == mid_train_ids) {
                  //说明是统一趟车
                  continue;
                } else {
                  bool find_to_train = false;
                  auto res = trains_basic.Search(mid_trains[j], find_to_train);
                  if (res[0].start_time.cmp <= date.cmp && res[0].end_time.cmp >= date.cmp) {
                    TrainInfoMost train_info_most_;
                    trains_most.read(res[0].pos, train_info_most_);
                    int start_pos, end_pos;
                    get_station_pos(train_info_most.station_names, train_info_most_.station_num, midStations[i],
                                    tStation, start_pos, end_pos);
                    if (start_pos <= end_pos) {
                      int date_num = date.minus_mon(date, res[0].start_time);
                      bool update_min = false;
                      int tmp;
                      if (mode) {
                        // mode = 1 time
                        int tmp = to_values[i] + train_info_most_.times[end_pos] - train_info_most_.times[start_pos];
                        if (tmp < ans) {
                          update_min = true;
                        } else if (tmp == ans) {
                          if (mid_train_ids < info1.train_id) {
                            update_min = true;
                          } else if (mid_train_ids == info1.train_id && mid_trains[j] < info2.train_id) {
                            update_min = true;
                          }
                        }
                      } else {
                        int tmp = to_values[i] + train_info_most_.prices[end_pos] - train_info_most_.prices[start_pos];
                        if (tmp < ans) {
                          update_min = true;
                        } else if (tmp == ans) {
                          if (mid_train_ids < info1.train_id) {
                            update_min = true;
                          } else if (mid_train_ids == info1.train_id && mid_trains[j] < info2.train_id) {
                            update_min = true;
                          }
                        }
                      }
                      if (update_min) {
                        ans = tmp;
                        mid = midStations[i];
                        info1.train_id = mid_train_ids;
                        info2.train_id = mid_trains[j];
                        info2.price = train_info_most.prices[end_pos] - train_info_most.prices[start_pos];
                        info2.from = train_info_most.leaving_time[start_pos];
                        info2.to = train_info_most.arriving_time[end_pos];
                        info2.left_seat = res[0].seat_num;
                        for (int i = start_pos; i < end_pos; i++) {
                          info2.left_seat = std::min(info2.left_seat, train_info_most.left_seats[date_num][i]);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          if (ans != INF) {
            bool find_train = false;
            auto res = trains_basic.Search(info1.train_id, find_train);
            if (find_train) {
              trains_most.read(res[0].pos, train_info_most);
              int start_pos, end_pos;
              get_station_pos(train_info_most.station_names, train_info_most.station_num, fStation, mid, start_pos,
                              end_pos);
              info1.from = train_info_most.leaving_time[start_pos];
              info1.to = train_info_most.arriving_time[end_pos];
              info1.left_seat = res[0].seat_num;
              int date_num = date.minus_mon(date, train_info_most.start_time);
              for (int i = start_pos; i < end_pos; i++) {
                info1.left_seat = std::min(info1.left_seat,train_info_most.left_seats[date_num][i]);
              }
              info1.price = train_info_most.prices[end_pos] - train_info_most.prices[start_pos];
            }
            std::cout << info1.train_id.train_id << " " << fromStation << " " << info1.from << " -> " << mid.train_name <<
            " " << info1.to <<
            " " << info1.price << " " << info1.left_seat << "\n";
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
                              const utils::Time &date_, int num, bool queue_, int timestamp, utils::Time &leaving_time,
                              utils::Time &arriving_time, int &price) {
    TrainId trainID(trainID_);
    StationName start_station(startStation_);
    StationName end_station(endStation_);
    TrainInfoBasic basic;
    bool find = false;
    basic = trains_basic.Search(trainID, find)[0];
    if (find) {
      if (basic.is_release && num <= basic.seat_num && basic.start_time.cmp <= date_.cmp && date_.cmp <= basic.end_time.
          cmp) {
        TrainInfoMost train_info_most;
        trains_most.read(basic.pos, train_info_most);
        int pos_start_station = -1, pos_end_station = -1;
        int permit_nums = basic.seat_num + 1;
        this->get_station_pos(train_info_most.station_names, train_info_most.station_num, start_station, end_station,
                              pos_start_station, pos_end_station);
        if (pos_start_station >= 0 && pos_end_station >= 0 && pos_start_station < pos_end_station) {
          int num_date = date_.minus_mon(date_, basic.start_time);
          price = train_info_most.prices[pos_end_station] - train_info_most.prices[pos_start_station];
          for (int i = pos_start_station; i < pos_end_station; i++) {
            permit_nums = std::min(permit_nums, train_info_most.left_seats[num_date][i]);
          }
          if (permit_nums < num) {
            if (queue_) {
              queue_key key_q;
              queue_value value_q;
              key_q.time_num = num_date;
              key_q.train_id = trainID;
              value_q.reversed_seats_num = num;
              value_q.time_stamp = timestamp;
              value_q.start_pos = pos_start_station;
              value_q.end_pos = pos_end_station;
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
            price = train_info_most.prices[pos_end_station] - train_info_most.prices[pos_start_station];
            leaving_time = train_info_most.leaving_time[pos_start_station];
            arriving_time = train_info_most.arriving_time[pos_end_station];
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
                                  const char *endStation_, const utils::Time &date_, int timestamp, int num,
                                  bool queue_, sjtu::vector<utils::transfer_union> &change) {
    TrainId trainID(trainID_);
    StationName start_station(startStation_);
    StationName end_station(endStation_);
    bool find = false;
    TrainInfoBasic info_basic(trains_basic.Search(trainID, find)[0]);
    if (find) {
      int timenum = date_.minus_mon(date_, info_basic.start_time);
      if (queue_) {
        //在队列中
        queues.Remove({trainID, timenum}, {num, timestamp, 0, 0});
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
        sjtu::vector<queue_value> res = queues.Search({trainID, timenum}, find);
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
              queues.Remove({trainID, timenum}, res[s]);
              for (int i = res[s].start_pos; i < res[s].end_pos; i++) {
                train_info_most.left_seats[timenum][i] -= res[s].reversed_seats_num;
              }
              change.push_back(utils::transfer_union(username_, res[s].time_stamp, 0));
            }
          }
          trains_most.update(info_basic.pos, train_info_most);
        }
      }
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
    }
  }
}

#endif //STATION_HPP
