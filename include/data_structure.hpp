#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using string = std::string;
using json = nlohmann::json;
using ifstream = std::ifstream;

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

  std::map<string, json> connection_registers_m;
  std::map<string, json> device_profiles_m;
  std::map<string, json> device_registers_m;
  std::map<string, json> format_profiles_m;

  void fill_maps() {
    ifstream connection_registers_f("./include/config/connection_registers.json");
    ifstream device_profiles_f("./include/config/device_profiles.json");
    ifstream device_registers_f("./include/config/device_registers.json");
    ifstream format_profiles_f("./include/config/format_profiles.json");

    json connection_registers_j = json::parse(connection_registers_f);
    json device_profiles_j = json::parse(device_profiles_f);
    json device_registers_j = json::parse(device_registers_f);
    json format_profiles_j = json::parse(format_profiles_f);

    for (const auto &item : connection_registers_j) {
      connection_registers_m[item["source"]] = item;
    }

    for (const auto &item : device_profiles_j) {
      device_profiles_m[item["name"]] = item;
    }

    for (const auto &item : device_registers_j) {
      device_registers_m[item["device"]] = item;
    }

    for (const auto &item : format_profiles_j) {
      format_profiles_m[item["name"]] = item;
    }
  }
};