#pragma once

#include "mosquitto.h"
#include "nlohmann/json.hpp"
#include "simplebluez/Bluez.h"
#include "RF24/RF24.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>
#include <map>
#include <termios.h>

namespace DataStructure {
  class Instance;

  struct TaskData {
    Instance *ds;
    std::string source;
    bool active;
  };

  class Instance {
    public:
      termios tty;
      int uart_fd;
      int i2c_fd;

      mosquitto *mosq = nullptr;
      RF24 radio = RF24(22, 0);

      std::map<std::string, std::string> universal_map;

      std::map<std::string, std::string> rx_rf24_map;
      std::map<std::string, std::string> tx_rf24_map;
      std::map<std::string, TaskData *> active_registers;
      std::vector<std::shared_ptr<SimpleBluez::Device>> ble_peripherals;
      std::vector<std::shared_ptr<SimpleBluez::Characteristic>> ble_characteristics;

      nlohmann::json ble_addresses;
      nlohmann::json mqtt_topics;

      Instance() { 
        fill_maps(); 

      }

      void fill_maps();
  };
} 
