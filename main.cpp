#include "ble_handler.hpp"
#include "data_structure.hpp"
#include "http_handler.hpp"
#include "mqtt_handler.hpp"
#include <future>
#include <iostream>
#include <map>
#include <thread>

int main() {
  spdlog::info("Walking by the wall");

  DataStructure dstructure;

  dstructure.fill_maps();

  spdlog::set_level(spdlog::level::debug);

  std::thread mqtt_thread(mqtt_handler, &dstructure);
  std::thread http_thread(http_handler, &dstructure);
  std::thread ble_thread(ble_handler, &dstructure);

  mqtt_thread.join();
  http_thread.join();
  ble_thread.join();

  return 0;
}
