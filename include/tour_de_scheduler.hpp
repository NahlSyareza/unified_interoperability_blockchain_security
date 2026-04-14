#pragma once

#include "data_structure.hpp"
#include "de_ruyter.hpp"
#include <functional>
#include <iostream>
#include <thread>

namespace TourDeScheduler {
inline void extract_config(std::string path, nlohmann::json *json_ptr) {
  std::ifstream file(path);
  *json_ptr = nlohmann::json::parse(file);
  file.close();
}

inline void create_task_register(std::string source, nlohmann::json *json_ptr) {
  nlohmann::json connection_registers, instance_registers, device_profiles, format_profiles;

  extract_config("./config/connection_registers.json", &connection_registers);
  extract_config("./config/instance_registers.json", &instance_registers);
  extract_config("./config/device_profiles.json", &device_profiles);
  extract_config("./config/format_profiles.json", &format_profiles);

  std::string destination = connection_registers[source]["destination"];

  (*json_ptr)["interval"] = connection_registers[source]["interval"];

  if (connection_registers[source].count("rules")) {
    (*json_ptr)["rules"] = connection_registers[source]["rules"];
  } else {
    (*json_ptr)["rules"] = "";
  }

  nlohmann::json data;
  data["name"] = source;

  if (!instance_registers.count(source)) {
    spdlog::error("Source instance {} is not recognized!", source);
    json_ptr->clear();
    return;
  } else if (!instance_registers.count(destination)) {
    spdlog::error("Destination instance {} is not recognized!", destination);
    json_ptr->clear();
    return;
  }

  std::string device = instance_registers[source]["device"];
  if (!device_profiles.count(device)) {
    spdlog::error("Source device profile {} is not found!", device);
    json_ptr->clear();
    return;
  }
  data["device"] = device_profiles[device];

  std::string format = instance_registers[source]["format"];
  if (!format_profiles.count(format)) {
    spdlog::error("Source format profile {} is nout found!", format);
    json_ptr->clear();
    return;
  }
  data["format"] = format_profiles[format];

  (*json_ptr)["src"] = data;

  data["name"] = destination;

  device = instance_registers[destination]["device"];
  if (!device_profiles.count(device)) {
    spdlog::error("Destination device profile {} is not found!", device);
    json_ptr->clear();
    return;
  }
  data["device"] = device_profiles[device];

  format = instance_registers[destination]["format"];
  if (!format_profiles.count(format)) {
    spdlog::error("Destination format profile {} is not found!", format);
    json_ptr->clear();
    return;
  }
  data["format"] = format_profiles[format];

  (*json_ptr)["dst"] = data;
}
}; // namespace TourDeScheduler

void generic_task_function(DataStructure::TaskData *td);
bool create_task_detached(DataStructure::Instance *ds, std::string task_name, std::string source);