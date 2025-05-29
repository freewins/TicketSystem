
//
// Created by Freewings on 2025/5/6.
//

#ifndef PAIR_HPP
#define PAIR_HPP
namespace sjtu{
  template<class T1, class T2>
  class pair {
  public:
    T1 first;
    T2 second;
    constexpr pair() : first(), second() {}
    pair(const pair &other) = default;
    pair(pair &&other) = default;
    pair(const T1 &x, const T2 &y) : first(x), second(y) {}
    template<class U1, class U2>
    pair(U1 &&x, U2 &&y) : first(x), second(y) {}
    template<class U1, class U2>
    pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
    template<class U1, class U2>
    pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}



    friend bool operator==(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      return p1.first==p.first && p1.second==p.second;
    }
    friend bool operator!=(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      return !(p1==p);
    }

    friend bool operator<(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      if(p1.first != p.first){
        return p1.first < p.first;
      }
      else{
        return p1.second < p.second;
      }
    }
    friend bool operator>(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      if(p1.first != p.first){
        return p.first < p1.first;
      }
      else{
        return p.second  < p1.second;
      }
    }

    pair<T1,T2> operator=(const pair<T1,T2> &p){
      if( *this != p){
        first=p.first;
        second=p.second;
      }
      return *this;
    }
  };
}


#endif //PAIR_HPP
