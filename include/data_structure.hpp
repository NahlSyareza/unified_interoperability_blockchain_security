#pragma once

#include <iostream>
#include <map>

using string = std::string;
// template <typename K, typename V> using map = std::map<K, V>;

class DataStructure {
public:
  struct Data {
    string payload;
  };

  std::map<string, Data *> http_map;
  std::map<string, Data *> mqtt_map;

  void insert_map_key(std::map<string, Data *> *mp, string key, string payload) {
    if (mp->find(key) == mp->end()) {
      // cout << "Do this once" << endl;
      (*mp)[key] = (Data *)calloc(1, sizeof(Data));
      new (&(*mp)[key]->payload) string();
    }

    (*mp)[key]->payload = payload;
  }
};