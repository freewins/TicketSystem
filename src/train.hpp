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
    struct TrainId {
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

      bool operator=(const StationName &t) const {

      }
      bool operator<(const StationName &t) const {
        return strcmp(train_name, t.train_name) < 0;
      }
    };

    struct Station {
      //Save in B+ Tree
      struct TrainPos {
        TrainId train_id;
        long long train_pos;
      } train_poses[30]; //Save the train that pass this station.
    };

    struct TrainInfo {
      // Save in the train information
      int seat_num = 0;
      int prices[30]{0};
      int stopper_times[30]{0};

      struct StationInfo {
        //For the train to check which station it passed.
        StationName station_name;
        long long storage_pos;
        int left_seats;
      } stations[30];

      utils::Time start_time, end_time;
      bool is_release = false;
      char Type;
    };

    BPlusTree<TrainInfo, TrainId, 40> trains;
    BPlusTree<Station, StationName, 40> stations;

    int total_trains = 0;

  public:
    //TODO add a total information file
    Train(): trains("../db/train_info.data"), stations("../db/station_info.data") {
    }

    //-----Admin-----

    bool AddTrain(const std::string &trainID_, const int &stationNum_, const int &seatNum_, const std::string &stations_,
                  const std::string &prices_, const std::string &
                  startTime_, const std::string &travelTimes_, const std::string &stopoverTimes_,
                  const std::string &saleData_, const char &type_);

    void ReleaseTrain(const std::string &trainID_);

    void DeleteTrain(const std::string &trainID_);

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

  }


  inline void Train::ReleaseTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    TrainInfo info(trains.Search(trainID,find)[0]);
    if (find) {
      if (info.is_release == false) {
        info.is_release = true;
        trains.Update(trainID, info);
      }
    }
  }
  //TODO
  inline void Train::DeleteTrain(const std::string &trainID_) {
    TrainId trainID(trainID_);
    bool find = false;
    TrainInfo info(trains.Search(trainID,find)[0]);
    if (find) {

    }
  }
}
#endif //STATION_HPP
