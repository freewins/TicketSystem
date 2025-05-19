//
// Created by Freewings on 2025/4/10.
//

#ifndef STATION_HPP
#define STATION_HPP
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

      TrainInfoBasic &operator=(const TrainInfoBasic &t) {
        is_release = t.is_release;
        seat_num = t.seat_num;
        start_time = t.start_time;
        end_time = t.end_time;
        pos = t.pos;
      }
    };

    struct TrainInfoMost {
      int station_num = 0;
      StationName station_names[MAX_TRAINS];

      int prices[MAX_TRAINS]{0};
      int stopper_times[MAX_TRAINS]{0};
      int travel_times[MAX_TRAINS]{0};

      int max_days = 0;
      //注意此处的值 left_seats[i][j]代表日期标号为 i 时,且当前火车编号为j,从j出发到j + 1站的座位数，0-based
      int left_seats[MAX_DAYS][MAX_TRAINS]{0};

      utils::Time start_time;
      char type;

      TrainInfoMost() = default;
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

    sjtu::vector<TrainId> QueryBasic(StationName fStation, StationName tStation,bool & find);

    /**
     *
     * @param fromStation 出发站
     * @param toStation 终点站
     * @param date 日期
     * @param mode 模式 0 默认为按照票价 1 为按照时间进行排序
     */
    void QueryTicket(const char * fromStation,const char * toStation,utils::Time date,int mode = 0);

    void QueryTrain();

    void QueryTransfer(const char * fromStation,const char * toStation,utils::Time date,int mode = 0);

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
      utils::split_int(train_info_most.prices, prices_, '|');
      for (int i = 1; i <= stationNum_; i++) {
        train_info_most.prices[i] += train_info_most.prices[i - 1];
      }
      //路程时间
      utils::split_int(train_info_most.stopper_times, stopoverTimes_, '|');
      //经停时间
      if (stationNum_ > 2) {
        utils::split_int(train_info_most.stopper_times, stopoverTimes_, '|');
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
    //TODO 如果没有找到 -> optional
    if (find) {
      if (!info_basic.is_release) {
        //TODO 思考是否需要进行删除
        TrainInfoMost train_info_most;
        trains_most.read(info_basic.pos, train_info_most);
        for (auto t: train_info_most.station_names) {
          stations.Remove(t, trainID);
        }
        trains_basic.Remove(trainID, info_basic);
        return true;
      }
    }
    return false;
  }

  inline sjtu::vector<Train::TrainId> Train::QueryBasic(StationName fStation, StationName tStation,bool & find) {
    sjtu::vector<Train::TrainId> res;
    sjtu::vector<Train::TrainId> fTrains;
    sjtu::vector<Train::TrainId> tTrains;

    find = false;
    fTrains = stations.Search(fStation,find);
    if (find) {
      tTrains = stations.Search(tStation,find);
      if (find) {
        int i = 0, j = 0;
        while (i < fTrains.size() && j < tTrains.size()) {
          if (fTrains[i] == tTrains[j]) {
            res.push_back(fTrains[i]);
          }
          else if (fTrains[j] < tTrains[i]) {
            j++;
          }
          else {
            i++;
          }
        }
      }
    }
    return res;
  }

  inline void Train::QueryTicket(const char *fromStation, const char *toStation, utils::Time date, int mode) {
    StationName fStation(fromStation);
    StationName tStation(toStation);
    bool find = false;
    sjtu::vector<Train::TrainId> path;
    sjtu::vector<long long> pos_vector;
    path = this->QueryBasic(fStation, tStation, find);
    if (find) {
      for (auto t: path) {
        bool find_ = false;
        TrainInfoBasic info(trains_basic.Search(t,find_)[0]);
         if (info.start_time.cmp <= date.cmp && info.end_time.cmp >= date.cmp) {
           pos_vector.push_back(info.pos);
         }
      }
    }
    TrainInfoMost train_info_most;
    int ans = 0x3f3f3f3f;
    for (auto t : pos_vector) {
      trains_most.read(t, train_info_most);
      int start_pos,end_pos;
      get_station_pos(train_info_most.station_names,train_info_most.station_num,fStation,tStation,start_pos,end_pos);
      if (mode) {//time
        //TODO 添加时间函数
      }
      else {//price
        ans = std::min(ans, train_info_most.prices[end_pos] - train_info_most.prices[start_pos]);
      }
    }
    return ;
  }
  //TODO
  inline void Train::QueryTransfer(const char *fromStation, const char *toStation, utils::Time date, int mode) {
    StationName fStation(fromStation);
    StationName tStation(toStation);
    //Step 1: 获得所有的 从fStation出发的点 ，然后获得在date内能到达的Stations
    //Step 2:
    //Step 2: 对于这些Station，再求从这些Station
  }




  //TODO 添加开始和结束时间
  inline int Train::BuyTicket(const char *username_, const char *trainID_, const char *startStation_,
                              const char *endStation_,
                              const utils::Time &date_, int num, bool queue_, int timestamp, utils::Time &leaving_time,
                              utils::Time &arriving_time, int &price) {
    TrainId trainID(trainID_);
    StationName start_station(startStation_);
    StationName end_station(endStation_);
    TrainInfoBasic basic;
    int price = 0;
    bool find = false;
    basic = trains_basic.Search(trainID, find)[0];
    if (find) {
      //TODO  封装到一个check函数里面
      if (basic.is_release && num <= basic.seat_num && basic.start_time.cmp <= date_.cmp && date_.cmp <= basic.end_time.
          cmp) {
        TrainInfoMost train_info_most;
        trains_most.read(basic.pos, train_info_most);
        int pos_start_station = -1, pos_end_station = -1;
        int permit_nums = basic.seat_num + 1;
        //TODO 是否需要封装成一个函数，找到对应的始发和终点站
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
          for (auto t: res) {
            bool change_ = true;
            for (int i = t.start_pos; i < t.end_pos; i++) {
              if (train_info_most.left_seats[timenum][i] < t.reversed_seats_num) {
                change_ = false;
                break;
              }
            }
            if (change_) {
              //说明可以进行修改
              queues.Remove({trainID, timenum}, t);
              for (int i = t.start_pos; i < t.end_pos; i++) {
                train_info_most.left_seats[timenum][i] -= t.reversed_seats_num;
              }
              change.push_back(utils::transfer_union(username_, t.time_stamp, 0));
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
          }
      else {
        return false;
      }
    } catch (std::filesystem::filesystem_error &e) {
      std::cout << e.what() << std::endl;
    }
  }

}

#endif //STATION_HPP
