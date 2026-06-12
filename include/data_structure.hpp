#pragma once

#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <simplebluez/Bluez.h>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <spdlog/spdlog.h>
#include <map>
#include <chrono>
#include <coap3/coap.h>

namespace DataStructure {
  class Instance;

  class Instance {
    public:
      std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
      std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
      bool pr_time = true;
      const int max_pr_time_count = 60;
      int pr_time_count = 0;
      std::vector<long> pr_time_sum;

      bool radio_mode = false;
      coap_context_t *coap_ctx = nullptr;
      coap_session_t *coap_sess = nullptr;

      mosquitto *mosq = nullptr;
      RF24 radio = RF24(22, 0);
      RF24Network radio_net = RF24Network(radio);

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
