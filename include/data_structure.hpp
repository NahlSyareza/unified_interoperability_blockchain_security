#pragma once

#include <iostream>
#include <map>

using namespace std;

class DataStructure {
public:
  struct Data {
    string payload;
  };

  map<string, Data *> http_map;
  map<string, Data *> mqtt_map;

  void insert_map_key(map<string, Data *> *mp, string key, string payload) {
    if (mp->find(key) == mp->end()) {
      // cout << "Do this once" << endl;
      (*mp)[key] = (Data *)calloc(1, sizeof(Data));
      new (&(*mp)[key]->payload) string();
    }

    (*mp)[key]->payload = payload;
  }
};