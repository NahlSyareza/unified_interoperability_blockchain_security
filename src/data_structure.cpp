#include "data_structure.hpp"
#include <fstream>

DataStructure::Instance::Instance() {
  std::ifstream _ble_addresses("./config/ble_addresses.json");
  std::ifstream _mqtt_topics("./config/mqtt_topics.json");

  ble_addresses = nlohmann::json::parse(_ble_addresses);
  mqtt_topics = nlohmann::json::parse(_mqtt_topics);
}

long DataStructure::Instance::time_now() {
  auto current_point = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(current_point - epoch_point);
  return dur.count();
}
