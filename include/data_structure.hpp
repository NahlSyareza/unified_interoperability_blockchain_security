#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simplebluez/Bluez.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>
#include <map>

namespace DataStructure {
class Instance;

struct TaskData {
  Instance *ds;
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

  // std::vector<SimpleBLE::Peripheral> peripherals;
  // std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  nlohmann::json ble_addresses;
  nlohmann::json mqtt_topics;

  Instance() { fill_maps(); }

  void fill_maps();
};
} // namespace DataStructure
