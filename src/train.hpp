//
// Created by Freewings on 2025/4/10.
//

#ifndef STATION_HPP
#define STATION_HPP
#include "BPlusTree.hpp"
#include "utils.hpp"

namespace train {
  //TODO Add the constructor
  class Train {
  private:
    const static int MAX_TRAINS = 26;
  private:

    struct TrainId { //The key to store the train
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
    };

    struct Station {
      //Save in B+ Tree
      TrainId train_id[MAX_TRAINS];
    };
    struct TrainInfoBasic {
      bool is_release = false;
      char type;
      int seat_num = 0;
      int station_num = 0;
      int prices[MAX_TRAINS]{0};
      int stopper_times[MAX_TRAINS]{0};
      int left_seats[MAX_TRAINS]{0};
      utils::Time start_time, end_time,time;

      TrainInfoBasic() = default;
      TrainInfoBasic(const TrainInfoBasic &t) {
        is_release = t.is_release;
        type = t.type;
        seat_num = t.seat_num;
        memcpy(prices, t.prices, sizeof(int) * MAX_TRAINS);
        memcpy(stopper_times, t.stopper_times, sizeof(int) * MAX_TRAINS);
        memcpy(left_seats, t.left_seats, sizeof(int) * MAX_TRAINS);
        start_time = t.start_time;
        end_time = t.end_time;
        time = t.time;
      }
      bool operator<(const TrainInfoBasic& t) const {
        return time < t.time;
      }
    };
    struct TrainInfoStations {
      int station_nums = 0;
      StationName station_names[MAX_TRAINS];

      bool operator<(const TrainInfoStations &t) const {
        return station_nums < t.station_nums;
      }
    };

    BPlusTree<TrainInfoBasic, TrainId, 40> trains_basic; //Store the basic train info
    BPlusTree<TrainInfoStations,TrainId, 40> trains_station; //Store the station name
    BPlusTree<Station, StationName, 40> stations; //Store the station info

    const std::string path_stations = "../db/station_info.data";
    const std::string path_trains_basic = "../db/train_basic_info.data";
    const std::string path_trains_stations = "../db/train_stations_info.data";
    int total_trains = 0;

  public:
    //TODO add a total information file
    Train(): stations(path_stations),trains_station(path_trains_stations),trains_basic(path_trains_basic) {
    }

    //-----Admin-----

    bool AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_, const std::string &stations_,
                  const std::string &prices_, const std::string &
                  startTime_, const std::string &travelTimes_, const std::string &stopoverTimes_,
                  const std::string &saleData_, const char &type_);

    bool ReleaseTrain(const std::string &trainID_);

    bool DeleteTrain(const std::string &trainID_);

    //-----User-----
    void QueryTrain();

    void QueryTransfer();

    void QueryTicket();

    //这两个函数必须被user的函数调用，也就是说user中必须存在对应的函数
    void BuyTicket();

    void RefundTicket();
  };
  //TODO
  inline bool Train::AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_, const std::string &stations_, const std::string &prices_, const std::string &startTime_, const std::string &travelTimes_, const std::string &stopoverTimes_, const std::string &saleData_, const char &type_) {
    TrainId trainID(trainID_);
    TrainInfoBasic train_info_basic;
    TrainInfoStations train_info_stations;
    train_info_basic.seat_num = seatNum_;
    train_info_basic.station_num = stationNum_;
    train_info_basic.type = type_;
    train_info_basic.time = utils::string_to_time(startTime_,1);
    { //处理车站
      std::vector<std::string> stations = utils::split(stations_, '|');
    }
    //车票价格
    utils::split_int(train_info_basic.prices,prices_, '|');
    //路程时间
    utils::split_int(train_info_basic.stopper_times,stopoverTimes_, '|');
    //经停时间
    if (stationNum_ > 2) {
      utils::split_int(train_info_basic.stopper_times,stopoverTimes_, '|');
    }
    //发行时间
    {
      int pos = saleData_.find('|',0);
      if (pos != std::string::npos) {
        train_info_basic.start_time = utils::string_to_time(saleData_.substr(0,pos),2);
        train_info_basic.end_time = utils::string_to_time(saleData_.substr(pos + 1),2);
      }
      else {
        return false;
      }
    }
    //执行插入操作
    if (trains_basic.Insert(trainID, train_info_basic)) {
      trains_station.Insert(trainID,train_info_stations);
      return true;
    }
    else {
      return false;
    }
  }


  inline bool Train::ReleaseTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    TrainInfoBasic info_basic (trains_basic.Search(trainID,find)[0]); //如果没有找到 -> optional
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
    TrainInfoBasic info_basic (trains_basic.Search(trainID,find)[0]); //如果没有找到 -> optional
    if (find) {
      if (!info_basic.is_release) {
        //TODO 必须优化 B+树是否要去掉value的删除
        trains_basic.Remove(trainID,info_basic);
        TrainInfoStations info_stations (trains_station.Search(trainID,find)[0]); //把每个车站的TrainId删掉吗
        trains_station.Remove(trainID,info_stations);
        return true;
      }
    }
    return false;
  }
}
#endif //STATION_HPP
