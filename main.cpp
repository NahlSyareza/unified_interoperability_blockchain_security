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

  DataStructure::Instance ds;

  // dstructure.fill_maps();

  spdlog::set_level(spdlog::level::debug);

  // nlohmann::json json_obj;
  // TourDeScheduler::create_task_register("device-1/test", &json_obj);

  // spdlog::debug("{}", json_obj.dump(1));

  // return 0;

  std::thread mqtt_thread(mqtt_handler, &ds);
  std::thread http_thread(http_handler, &ds);
  std::thread ble_thread(ble_handler, &ds);

  mqtt_thread.join();
  http_thread.join();
  ble_thread.join();

  return 0;
}
