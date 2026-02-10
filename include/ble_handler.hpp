#pragma once

#include <chrono>
#include <data_structure.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <simpleble/SimpleBLE.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <utils.hpp>
#include <vector>

using json = nlohmann::json;

// static std::vector<SimpleBLE::Peripheral> peripherals;
// static std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

// void ble_write_request(std::string identifier, std::string msg) {
//   for (auto &p : peripherals) {
//     if (p.identifier() == identifier)
//       p.write_request(uuid_pair[identifier].first, uuid_pair[identifier].second, msg);
//   }
// }

void dummy_write(DataStructure *dstructure) {
  // std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuid_pair;

  SimpleBLE::ByteArray by1 = "Give up free will forever";
  SimpleBLE::ByteArray by2 = "Their voices won't be heard at all";

  for (auto &p : dstructure->peripherals) {
    std::string iden = p.identifier();
    if (p.identifier() == "ROG Phone 9 FE")
      p.write_request(dstructure->uuid_pair[iden].first, dstructure->uuid_pair[iden].second, by1);
    else if (p.identifier() == "Who is this")
      p.write_request(dstructure->uuid_pair[iden].first, dstructure->uuid_pair[iden].second, by2);
  }

  spdlog::info("Hosoi komichi o aruki");

  for (auto &p : dstructure->peripherals) {
    p.disconnect();
  }
}

int ble_handler(DataStructure *dstructure) {
  std::optional<SimpleBLE::Adapter> adapter_optional = Utils::getAdapter();

  if (!adapter_optional.has_value()) {
    return EXIT_FAILURE;
  }

  auto adapter = adapter_optional.value();

  adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral p) {
    if ((dstructure->ble_addresses.find(p.address()) != dstructure->ble_addresses.end() ||
         dstructure->ble_addresses.find(p.identifier()) != dstructure->ble_addresses.end()) &&
        p.is_connectable()) {
      spdlog::info("Found device {} ({})", p.identifier(), p.address());
      dstructure->peripherals.push_back(p);
    }
  });

  adapter.scan_for(3000);

  std::cout << "The following devices were found:" << std::endl;
  for (size_t i = 0; i < dstructure->peripherals.size(); i++) {
    std::cout << "[" << i << "] " << dstructure->peripherals[i].identifier() << " [" << dstructure->peripherals[i].address() << "]" << std::endl;
  }

  for (auto &p : dstructure->peripherals) {
    p.connect();
  }

  for (const auto &[k, v] : dstructure->ble_addresses) {
    std::string service_string = v["service"];
    std::string characteristic_string = v["characteristic"];
    spdlog::info("Make pair for {}", k);
    SimpleBLE::BluetoothUUID service(service_string);
    SimpleBLE::BluetoothUUID characteristic(characteristic_string);
    dstructure->uuid_pair[k] = std::make_pair(service, characteristic);
  }

  // for (const auto &[k, v] : uuid_pair) {
  //   spdlog::info("{}: {} {}", k, v.first, v.second);
  // }

  // dummy_write();

  return EXIT_SUCCESS;
}