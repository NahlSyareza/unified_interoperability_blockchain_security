#include "ble_handler.hpp"
#include <simplebluez/Exceptions.h>
#include "data_route_handler.hpp"

SimpleBluez::Bluez bluez;
bool should_run = true;

void bluez_async_function() {
  while (should_run) {
    bluez.run_async();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::cout << "This task has ended" << std::endl;
}

void bluez_async_write() {

}

int ble_handler(DataStructure::Instance *ds) {
  bluez.init();

  // std::thread *bluez_async_thread = new std::thread(bluez_async_function, &bluez, &should_run);
  std::thread *bluez_async_thread = new std::thread(bluez_async_function);
  // bluez_async_thread.detach();

  std::vector<std::shared_ptr<SimpleBluez::Adapter>> adapters = bluez.get_adapters();
  std::shared_ptr<SimpleBluez::Adapter> adapter = adapters.at(0);

  SimpleBluez::Adapter::DiscoveryFilter filter;
  filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
  adapter->discovery_filter(filter);

  std::vector<std::shared_ptr<SimpleBluez::Device>> peripherals;

  adapter->set_on_device_updated([&peripherals, ds](std::shared_ptr<SimpleBluez::Device> peripheral) {
      if (ds->ble_addresses.count(peripheral->name()) && std::find(peripherals.begin(), peripherals.end(), peripheral) == peripherals.end()) {
      std::cout << "Detected: " << peripheral->name() << std::endl;
      peripherals.push_back(peripheral);
      ds->ble_peripherals.push_back(peripheral);
      }
      });

  adapter->discovery_start();
  spdlog::info("BLE discovery start");
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  adapter->discovery_stop();
  spdlog::info("BLE discovery stop");

  for (auto &peripheral : peripherals) {
    spdlog::info("Attempting to connect to {}", peripheral->name());

    nlohmann::json selected_peripheral = ds->ble_addresses[peripheral->name()];
    std::string name = peripheral->name();

    while (!peripheral->connected()) {
      try {
        peripheral->connect();
      } catch (SimpleDBus::Exception::SendFailed &e) {
        // std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }

    spdlog::info("Discovering services...");

    while (!peripheral->services_resolved()) {
      // std::cout << ".";
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    spdlog::info("Connected to {}", peripheral->name());

    std::shared_ptr<SimpleBluez::Characteristic> characteristic;

    std::string service_uuid = selected_peripheral["service"];
    std::string characteristic_uuid = selected_peripheral["characteristic"];

    try {
      auto service = peripheral->get_service(service_uuid);

      try {
        characteristic = service->get_characteristic(characteristic_uuid);
      } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
      }
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    characteristic->set_on_value_changed([name, ds](SimpleBluez::ByteArray new_value) {
        std::string payload(new_value.begin(), new_value.end());
        if(ds->pr_time) { 
        auto current_point = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(current_point - ds->epoch_point);
        ds->start_time = dur.count();
        }
        // std::cout << "Notified: " << payload << std::endl;
        ds->universal_map["ble/" + name] = payload;
        data_route_handler(ds, name);
        // spdlog::debug("(BLE) {}", payload);
        // std::cout << "Message arrived" << std::endl;
        // if (!ds->active_registers.count(name)) {
        // create_task_detached(ds, name);
        // }
        });

    characteristic->start_notify();

    spdlog::info("Connection established with {}", peripheral->name());
  }

  // spdlog::info("Peripheral disconnected");
  // should_run = false;
  bluez_async_thread->join();

  return 0;
}
