#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using string = std::string;
using json = nlohmann::json;
using ifstream = std::ifstream;
template <typename K, typename V> using map = std::map<K, V>;

int main() {
  ifstream device_profiles_file("./include/device_profiles.json");
  ifstream device_registers_file("./include/device_registers.json");
  ifstream connection_registers_file("./include/connection_registers.json");

  map<string, json> device_profiles_map, device_registers_map, connection_registers_map;

  json device_profiles = json::parse(device_profiles_file);
  json device_registers = json::parse(device_registers_file);
  json connection_registers = json::parse(connection_registers_file);

  for (const auto &item : device_profiles) {
    device_profiles_map[item["name"]] = item;
  }

  for (const auto &item : device_registers) {
    device_registers_map[item["device"]] = item;
  }

  for (const auto &item : connection_registers) {
    connection_registers_map[item["source"]] = item;
  }

  return 0;
}

