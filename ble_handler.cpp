#include "ble_handler.hpp"
#include "de_ruyter.hpp"
#include "nlohmann/json.hpp"
#include "simpleble/SimpleBLE.h"
#include "spdlog/spdlog.h"
#include "utils.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

using json = nlohmann::json;

int ble_handler(DataStructure *dstructure) {
  // std::optional<SimpleBLE::Adapter> adapter_optional = Utils::getAdapter();
  auto adapters = SimpleBLE::Adapter::get_adapters();

  // if (!adapter_optional.has_value()) {
  //   return EXIT_FAILURE;
  // }

  if (adapters.empty()) {
    spdlog::error("No adapter was found");
    return EXIT_FAILURE;
  }

  // auto adapter = adapter_optional.value();
  auto adapter = adapters.at(0);

  adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral p) {
    if ((dstructure->ble_addresses.find(p.address()) != dstructure->ble_addresses.end() || dstructure->ble_addresses.find(p.identifier()) != dstructure->ble_addresses.end()) && p.is_connectable()) {
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

  for (auto &p : dstructure->peripherals) {
    json listed_peripherals = dstructure->ble_addresses[p.identifier()];
    std::string service_string = listed_peripherals["service"];
    std::string characteristic_string = listed_peripherals["characteristic"];
    spdlog::info("Make pair for {} ({} {})", p.identifier(), service_string, characteristic_string);
    SimpleBLE::BluetoothUUID service(service_string);
    SimpleBLE::BluetoothUUID characteristic(characteristic_string);
    dstructure->uuid_pair[p.identifier()] = std::make_pair(service, characteristic);
  }

  for (auto &p : dstructure->peripherals) {
    spdlog::info("Creating BLE notifications");
    const std::string identifier = p.identifier();
    p.notify(dstructure->uuid_pair[identifier].first, dstructure->uuid_pair[identifier].second, [identifier, dstructure](SimpleBLE::ByteArray payload) {
      std::string string_payload(payload.begin(), payload.end());
      spdlog::info("BLE Notify: {} ({})", string_payload, identifier);
      de_ruyter(dstructure, identifier, string_payload);
    });
  }

  return EXIT_SUCCESS;
}

void dummy_write(DataStructure *dstructure) {

  SimpleBLE::ByteArray by1 = "Give up free will forever";
  SimpleBLE::ByteArray by2 = "Their voices won't be heard at all";

  for (auto &p : dstructure->peripherals) {
    std::string iden = p.identifier();
    if (p.identifier() == "ROG Phone 9 FE")
      p.write_request(dstructure->uuid_pair[iden].first, dstructure->uuid_pair[iden].second, by1);
    else if (p.identifier() == "BLE_DEV_0")
      p.write_request(dstructure->uuid_pair[iden].first, dstructure->uuid_pair[iden].second, by2);
  }

  spdlog::info("Hosoi komichi o aruki");

  for (auto &p : dstructure->peripherals) {
    p.disconnect();
  }
}