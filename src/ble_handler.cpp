#include "ble_handler.hpp"

int ble_handler(DataStructure *ds) {
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

  std::vector<SimpleBLE::Peripheral> scanned_peripherals;
  std::vector<SimpleBLE::Peripheral> connected_peripherals;

  adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral p) {
    if (ds->ble_addresses.count(p.identifier()) > 0 && p.is_connectable()) {
      spdlog::info("Found device {} ({})", p.identifier(), p.address());
      ds->peripherals.push_back(p);
      scanned_peripherals.push_back(p);
    }
  });

  adapter.set_callback_on_scan_start([]() { std::cout << "Scan started." << std::endl; });
  adapter.set_callback_on_scan_stop([]() { std::cout << "Scan stopped." << std::endl; });

  // adapter.scan_for(3000);
  adapter.scan_start();

  while (adapter.scan_is_active()) {
    if (!scanned_peripherals.empty()) {
      for (auto &scanned_peripheral : scanned_peripherals) {
        scanned_peripheral.connect();
        spdlog::info("(BLE) Connected to {}", scanned_peripheral.identifier());
        connected_peripherals.push_back(scanned_peripheral);
        scanned_peripherals.pop_back();
      }
    }

    if (!connected_peripherals.empty()) {
      for (auto &connected_peripheral : connected_peripherals) {
        spdlog::debug("(BLE) Registering pairs");
        nlohmann::json ble_address = ds->ble_addresses[connected_peripheral.identifier()];
        std::string identifier = connected_peripheral.identifier();
        std::string service = ble_address["service"];
        std::string characteristic = ble_address["characteristic"];
        SimpleBLE::BluetoothUUID service_uuid(service);
        SimpleBLE::BluetoothAddress characteristic_uuid(characteristic);
        ds->uuid_pair[identifier] = std::make_pair(service_uuid, characteristic_uuid);

        spdlog::debug("(BLE) Defining notifications {} {}", service, characteristic);
        try {
          connected_peripheral.notify(service_uuid, characteristic_uuid, [identifier, ds](SimpleBLE::ByteArray rx) {
            std::string payload(rx.begin(), rx.end());
            spdlog::info("(BLE) From {}: {}", identifier, payload);
            ds->ble_map[identifier] = payload;
            // de_ruyter(ds, identifier, payload_str);
          });
        } catch (const std::exception &e) {
          spdlog::error("(BLE) Error: {}", e.what());
        }
        connected_peripherals.pop_back();
      }
    }
  }

  return EXIT_SUCCESS;
}