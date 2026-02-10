#pragma once

#include <fstream>
#include <iostream>
#include <map>
// #include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <simpleble/SimpleBLE.h>

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

  std::vector<SimpleBLE::Peripheral> peripherals;
  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  void insert_map_key(std::map<string, Data *> *mp, string key, string payload) {
    if (mp->find(key) == mp->end()) {
      // cout << "Do this once" << endl;
      (*mp)[key] = (Data *)calloc(1, sizeof(Data));
      new (&(*mp)[key]->payload) string();
    }

    (*mp)[key]->payload = payload;
  }

  std::map<string, json> connection_registers;
  std::map<string, json> device_profiles;
  std::map<string, json> device_registers;
  std::map<string, json> format_profiles;

  std::map<string, json> ble_addresses;

  void fill_maps() {
    ifstream connection_registers_stream("./include/config/connection_registers.json");
    ifstream device_profiles_stream("./include/config/device_profiles.json");
    ifstream device_registers_stream("./include/config/device_registers.json");
    ifstream format_profiles_stream("./include/config/format_profiles.json");
    ifstream ble_addresses_stream("./include/config/ble_addresses.json");

    json parse_connection_registers = json::parse(connection_registers_stream);
    json parse_device_profiles = json::parse(device_profiles_stream);
    json parse_device_registers = json::parse(device_registers_stream);
    json parse_format_profiles = json::parse(format_profiles_stream);
    json parse_ble_addresses = json::parse(ble_addresses_stream);

    for (const auto &item : parse_connection_registers) {
      connection_registers[item["source"]] = item;
    }

    for (const auto &item : parse_device_profiles) {
      device_profiles[item["name"]] = item;
    }

    for (const auto &item : parse_device_registers) {
      device_registers[item["device"]] = item;
    }

    for (const auto &item : parse_format_profiles) {
      format_profiles[item["name"]] = item;
    }

    for (const auto &item : parse_ble_addresses) {
      ble_addresses[item["identifier"]] = item;
    }
  }
};