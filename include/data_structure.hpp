#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simpleble/SimpleBLE.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>
#include <map>

class DataStructure {
public:
  struct mosquitto *mosq = nullptr;

  std::map<std::string, std::string> http_map;
  std::map<std::string, std::string> mqtt_map;

  std::map<std::string, std::thread *> active_registers;

  std::vector<SimpleBLE::Peripheral> peripherals;
  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  nlohmann::json connection_registers;
  nlohmann::json device_profiles;
  nlohmann::json instance_registers;
  nlohmann::json format_profiles;
  nlohmann::json ble_addresses;
  nlohmann::json mqtt_topics;

  // T is of std::pair<string, json*>
  template <typename... T> void populate(nlohmann::json *base, std::string identifier, nlohmann::json *save, T... t) {
    nlohmann::json obj = (*base)[identifier];

    (*save)["name"] = identifier;

    (
        [&](auto &item [[maybe_unused]]) {
          std::string first = t.first;
          nlohmann::json second = *t.second;

          std::string value = obj[first];

          if (second.count(value) < 1) {
            spdlog::error("(DataStructure) Illegal action: Given key is not valid populate method. Identifier: {}", identifier);
            return;
          }

          nlohmann::json ref_obj = second[value];

          (*save)[first] = ref_obj;
        }(t),
        ...);
  }

  void fill_maps() {
    std::ifstream _connection_registers("./config/connection_registers.json");
    std::ifstream _device_profiles("./config/device_profiles.json");
    std::ifstream _instance_registers("./config/instance_registers.json");
    std::ifstream _format_profiles("./config/format_profiles.json");
    std::ifstream _ble_addresses("./config/ble_addresses.json");
    std::ifstream _mqtt_topics("./config/mqtt_topics.json");

    connection_registers = nlohmann::json::parse(_connection_registers);
    device_profiles = nlohmann::json::parse(_device_profiles);
    instance_registers = nlohmann::json::parse(_instance_registers);
    format_profiles = nlohmann::json::parse(_format_profiles);
    ble_addresses = nlohmann::json::parse(_ble_addresses);
    mqtt_topics = nlohmann::json::parse(_mqtt_topics);
  }
};