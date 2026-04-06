#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simpleble/SimpleBLE.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>
#include <map>

using string = std::string;
using json = nlohmann::json;
using ifstream = std::ifstream;

class DataStructure {
public:
  struct mosquitto *mosq = NULL;

  std::map<string, string> http_map;

  std::vector<SimpleBLE::Peripheral> peripherals;
  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  json connection_registers;
  json device_profiles;
  json instance_registers;
  json format_profiles;
  json ble_addresses;

  // T is of std::pair<string, json*>
  template <typename... T> void populate(json *base, string identifier, json *save, T... t) {
    json obj = (*base)[identifier];

    (*save)["name"] = identifier;

    (
        [&](auto &item) {
          string first = t.first;
          json second = *t.second;

          string value = obj[first];

          if (second.count(value) < 1) {
            spdlog::error("(DataStructure) Illegal action: Given key is not valid populate method. Identifier: {}", identifier);
            return;
          }

          json ref_obj = second[value];

          (*save)[first] = ref_obj;
        }(t),
        ...);
  }

  void fill_maps() {
    ifstream _connection_registers("./config/connection_registers.json");
    ifstream _device_profiles("./config/device_profiles.json");
    ifstream _instance_registers("./config/instance_registers.json");
    ifstream _format_profiles("./config/format_profiles.json");
    ifstream _ble_addresses("./config/ble_addresses.json");

    connection_registers = json::parse(_connection_registers);
    device_profiles = json::parse(_device_profiles);
    instance_registers = json::parse(_instance_registers);
    format_profiles = json::parse(_format_profiles);
    ble_addresses = json::parse(_ble_addresses);
  }
};