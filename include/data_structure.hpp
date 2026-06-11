#pragma once

#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <simplebluez/Bluez.h>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <spidev_lib++.h>
#include <spdlog/spdlog.h>
#include <map>
#include <termios.h>
#include <httplib.h>
#include <chrono>
#include <coap3/coap.h>

namespace DataStructure {
  class Instance;

  // struct TaskData {
  //   Instance *ds;
  //   std::string source;
  //   bool active;
  // };

  class Instance {
    public:
      // std::chrono::time_point<std::chrono::high_resolution_clock> epoch_point = std::chrono::high_resolution_clock::now();
      std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
      std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
      // long start_time = 0;
      // long end_time = 0;
      bool pr_time = true;
      const int max_pr_time_count = 60;
      int pr_time_count = 0;
      std::vector<long> pr_time_sum;

      termios tty;
      bool radio_mode = false;
      coap_context_t *coap_ctx = nullptr;
      coap_session_t *coap_sess = nullptr;
      int uart_h;
      int i2c_h;
      // httplib::Server svr;
      mosquitto *mosq = nullptr;
      RF24 radio = RF24(22, 0);
      RF24Network radio_net = RF24Network(radio);

      spi_config_t spi_config_settings = {
        .mode = 0,
        .bits_per_word = 8,
        .speed = 1000000,
        .delay = 0
      };
      SPI spi_s = SPI("/dev/spidev0.0", &spi_config_settings);
      SPI spi_d = SPI("/dev/spidev0.1", &spi_config)settings);

      std::map<std::string, std::string> universal_map;
      std::map<std::string, nlohmann::json> interop_map;

      std::vector<std::shared_ptr<SimpleBluez::Device>> ble_peripherals;
      std::vector<std::shared_ptr<SimpleBluez::Characteristic>> ble_characteristics;

      nlohmann::json ble_addresses;
      nlohmann::json mqtt_topics;

      void save_pr_time();

      Instance();
  };
} 
