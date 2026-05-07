#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simplebluez/Bluez.h"
#include "RF24/RF24.h"
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
      std::map<std::string, std::string> rx_rf24_map;
      std::map<std::string, std::string> tx_rf24_map;
      
      RF24 radio = RF24(22, 0);

      std::map<std::string, TaskData *> active_registers;

      std::vector<std::shared_ptr<SimpleBluez::Device>> ble_peripherals;
      std::vector<std::shared_ptr<SimpleBluez::Characteristic>> ble_characteristics;
      // std::vector<SimpleBLE::Peripheral> peripherals;
      // std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

      nlohmann::json ble_addresses;
      nlohmann::json mqtt_topics;

      Instance() { fill_maps(); }

      void fill_maps();
  };
} // namespace DataStructure
