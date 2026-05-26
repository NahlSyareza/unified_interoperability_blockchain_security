#pragma once

#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <simplebluez/Bluez.h>
#include <RF24/RF24.h>
#include <spidev_lib++.h>
#include <spdlog/spdlog.h>
// #include <fstream>
// #include <iostream>
#include <map>
#include <termios.h>
#include <httplib.h>
#include <chrono>

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
      const std::chrono::time_point<std::chrono::high_resolution_clock> epoch_point = std::chrono::high_resolution_clock::now();
      int uart_h;
      int i2c_h;
      httplib::Server svr;
      mosquitto *mosq = nullptr;
      RF24 radio = RF24(22, 0);
      // RF24 radio;
      spi_config_t spi_config_settings = {
        .mode = 0,
        .bits_per_word = 8,
        .speed = 1000000,
        .delay = 0
      };
      //      spi_config_settings.mode = 0;
      //      spi_config_settings.speed = 1000000;
      //      spi_config_settings.delay = 0;
      //      spi_config_settings.bits_per_word = 8;
      SPI spi_h = SPI("/dev/spidev0.0", &spi_config_settings);;

      std::map<std::string, std::string> universal_map;

      std::map<std::string, std::string> rx_rf24_map;
      std::map<std::string, std::string> tx_rf24_map;
      std::map<std::string, TaskData *> active_registers;
      std::vector<std::shared_ptr<SimpleBluez::Device>> ble_peripherals;
      std::vector<std::shared_ptr<SimpleBluez::Characteristic>> ble_characteristics;

      nlohmann::json ble_addresses;
      nlohmann::json mqtt_topics;

      Instance();
      long time_now();
  };
} 
