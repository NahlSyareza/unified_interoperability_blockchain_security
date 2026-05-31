#include "data_structure.hpp"
#include <fstream>

DataStructure::Instance::Instance() {
  std::ifstream _ble_addresses("./config/ble_addresses.json");
  std::ifstream _mqtt_topics("./config/mqtt_topics.json");

  ble_addresses = nlohmann::json::parse(_ble_addresses);
  mqtt_topics = nlohmann::json::parse(_mqtt_topics);

  _mqtt_topics.close();
  _ble_addresses.close();
}
