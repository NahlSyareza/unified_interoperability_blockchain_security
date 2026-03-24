#pragma once

#include <fstream>
#include <iostream>
#include <map>
// #include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <simpleble/SimpleBLE.h>
#include <spdlog/spdlog.h>

using string = std::string;
using json = nlohmann::json;
using ifstream = std::ifstream;

class DataStructure {
public:
  struct mosquitto *mosq = NULL;

  std::map<string, string> http_map;

  std::vector<SimpleBLE::Peripheral> peripherals;
  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  std::map<string, json> connection_registers;
  std::map<string, json> device_profiles;
  std::map<string, json> instance_registers;
  std::map<string, json> format_profiles;
  std::map<string, json> ble_addresses;

  void fill_maps() {
    ifstream connection_registers_stream("./config/connection_registers.json");
    ifstream device_profiles_stream("./config/device_profiles.json");
    ifstream instance_registers_stream("./config/instance_registers.json");
    ifstream format_profiles_stream("./config/format_profiles.json");
    ifstream ble_addresses_stream("./config/ble_addresses.json");

    json parse_connection_registers = json::parse(connection_registers_stream);
    json parse_device_profiles = json::parse(device_profiles_stream);
    json parse_instance_registers = json::parse(instance_registers_stream);
    json parse_format_profiles = json::parse(format_profiles_stream);
    json parse_ble_addresses = json::parse(ble_addresses_stream);

    for (const auto &item : parse_format_profiles) {
      format_profiles[item["name"]] = item;
    }

    for (const auto &item : parse_device_profiles) {
      device_profiles[item["name"]] = item;
    }

    for (const auto &item : parse_ble_addresses) {
      ble_addresses[item["identifier"]] = item;
    }

    for (const auto &item : parse_instance_registers) {
      // if (!device_profiles.count(item["profile"])) {
      //   string profile = item["profile"];
      //   spdlog::error("(Device Registers) Undetected profile: {}", profile);
      //   continue;
      // }

      instance_registers[item["name"]] = item;
    }

    for (const auto &item : parse_connection_registers) {
      if (!instance_registers.count(item["source"])) {
        string source = item["source"];
        spdlog::error("(Connection Registers) Undetected source device: {}", source);
        continue;
      }

      if (!instance_registers.count(item["destination"])) {
        string destination = item["destination"];
        spdlog::error("(Connection Registers) Undetected destination device: {}", destination);
        continue;
      }

      connection_registers[item["source"]] = item;
    }
  }
};