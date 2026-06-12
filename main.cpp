#include "ble_handler.hpp"
#include "data_structure.hpp"
#include "mqtt_handler.hpp"
#include "rf24_handler.hpp"
#include "coap_handler.hpp"
#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>

int main() {
  spdlog::info("Walking by the wall");

  DataStructure::Instance ds;

  spdlog::set_level(spdlog::level::debug);

  std::thread coap_th(coap_handler, &ds);
  std::thread rf24_th(rf24_handler, &ds);
  std::thread mqtt_th(mqtt_handler, &ds);
  std::thread ble_th(ble_handler, &ds);

  coap_th.join();
  rf24_th.join();
  mqtt_th.join();
  ble_th.join();

  return 0;
}
