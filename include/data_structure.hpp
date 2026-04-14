#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simpleble/SimpleBLE.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>
#include <map>

namespace DataStructure {
class Instance;

struct TaskData {
  Instance *ds;
  std::string task_name;
  std::string source;
  bool active;
};

class Instance {
public:
  struct mosquitto *mosq = nullptr;

  std::map<std::string, std::string> http_map;
  std::map<std::string, std::string> mqtt_map;
  std::map<std::string, std::string> ble_map;
  std::map<std::string, std::string> lora_map;

  std::map<std::string, TaskData *> active_registers;

  std::vector<SimpleBLE::Peripheral> peripherals;
  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  nlohmann::json ble_addresses;
  nlohmann::json mqtt_topics;

  Instance() { fill_maps(); }

  // T is of std::pair<string, json*>
  // template <typename... T> void populate(nlohmann::json *base, std::string identifier, nlohmann::json *save, T... t) {
  //   if (!base->count(identifier)) {
  //     spdlog::error("(DataStructure) Identifier not found. Perhaps your device name is wrong?");
  //     return;
  //   }

  //   nlohmann::json obj = (*base)[identifier];

  //   (*save)["name"] = identifier;

  //   (
  //       [&](auto &item [[maybe_unused]]) {
  //         std::string first = t.first;
  //         nlohmann::json second = *t.second;

  //         std::string value = obj[first];

  //         if (!second.count(value)) {
  //           spdlog::error("(DataStructure) Illegal action: Given key is not valid populate method. Identifier: {} Key: {}", identifier, value);
  //           return;
  //         }

  //         nlohmann::json ref_obj = second[value];

  //         (*save)[first] = ref_obj;
  //       }(t),
  //       ...);
  // }

  void fill_maps() {
    std::ifstream _ble_addresses("./config/ble_addresses.json");
    std::ifstream _mqtt_topics("./config/mqtt_topics.json");

    ble_addresses = nlohmann::json::parse(_ble_addresses);
    mqtt_topics = nlohmann::json::parse(_mqtt_topics);
  }
};
} // namespace DataStructure
