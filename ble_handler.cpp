// #define ESP32_ADDR "14:2b:2f:c4:f3:6e"
// #define ESP32_SRV_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define ESP32_CHR_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// #define PHONE_SRV_UUID "0000ff02-0000-1000-8000-00805f9b34fb"
// #define PHONE_CHR_UUID "0000ff02-0000-1000-8000-00805f9b34fb"

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

int main() {
  DataStructure dstructure;

  dstructure.fill_maps();

  std::optional<SimpleBLE::Adapter> adapter_optional = Utils::getAdapter();

  if (!adapter_optional.has_value()) {
    return EXIT_FAILURE;
  }

  auto adapter = adapter_optional.value();

  std::vector<SimpleBLE::Peripheral> peripherals;

  adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral p) {
    if ((dstructure.ble_addresses.find(p.address()) != dstructure.ble_addresses.end() ||
         dstructure.ble_addresses.find(p.identifier()) != dstructure.ble_addresses.end()) &&
        p.is_connectable()) {
      spdlog::info("Found device {} ({})", p.identifier(), p.address());
      peripherals.push_back(p);
    }
  });

  adapter.scan_for(3000);

  std::cout << "The following devices were found:" << std::endl;
  for (size_t i = 0; i < peripherals.size(); i++) {
    std::cout << "[" << i << "] " << peripherals[i].identifier() << " [" << peripherals[i].address() << "]" << std::endl;
  }

  for (auto &p : peripherals) {
    p.connect();
  }

  std::map<std::string, std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> kyuukutsu;
  for (const auto &[k, v] : dstructure.ble_addresses) {
    // json deserialize = v;
    std::string service_string = v["service"];
    std::string characteristic_string = v["characteristic"];
    spdlog::info("Make pair for {}", k);
    SimpleBLE::BluetoothUUID service(service_string);
    SimpleBLE::BluetoothUUID characteristic(characteristic_string);
    kyuukutsu[k] = std::make_pair(service, characteristic);
  }

  // for (auto &p : peripherals) {
  //   for (auto s : p.services()) {
  //     for (auto c : s.characteristics()) {
  //       if (s.uuid() == PHONE_SRV_UUID && c.uuid() == PHONE_CHR_UUID) {
  //         kyuukutsu[p.identifier()] = std::make_pair(s.uuid(), c.uuid());
  //       }
  // } else if (p.identifier() == "Who is this" && s.uuid() == ESP32_SRV_UUID && c.uuid() == ESP32_CHR_UUID) {
  //   kyuukutsu[p.identifier()] = std::make_pair(s.uuid(), c.uuid());
  // }
  //     }
  //   }
  // }

  for (const auto &[k, v] : kyuukutsu) {
    spdlog::info("{}: {} {}", k, v.first, v.second);
  }

  SimpleBLE::ByteArray by1 = "The unenlightened masses";
  SimpleBLE::ByteArray by2 = "They cannot make the judgement call";

  for (auto &p : peripherals) {
    std::string iden = p.identifier();
    if (p.identifier() == "ROG Phone 9 FE")
      p.write_request(kyuukutsu[iden].first, kyuukutsu[iden].second, by1);
    else if (p.identifier() == "Who is this")
      p.write_request(kyuukutsu[iden].first, kyuukutsu[iden].second, by2);
  }

  spdlog::info("Hosoi komichi o aruki");

  for (auto &p : peripherals) {
    p.disconnect();
  }

  return EXIT_SUCCESS;
}