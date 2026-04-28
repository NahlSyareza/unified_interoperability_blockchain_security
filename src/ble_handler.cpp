#include "ble_handler.hpp"
// #include "simpleble/Backend.h"

SimpleBluez::Bluez bluez;
bool should_run = true;

void bluez_async_function() {
  while (should_run) {
    bluez.run_async();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  std::cout << "This task has ended" << std::endl;
}

int ble_handler(DataStructure::Instance *ds [[maybe_unused]]) {
  std::ifstream input("./config/ble_addresses.json");
  nlohmann::json addresses = nlohmann::json::parse(input);

  bluez.init();

  // std::thread *bluez_async_thread = new std::thread(bluez_async_function, &bluez, &should_run);
  std::thread *bluez_async_thread = new std::thread(bluez_async_function);
  // bluez_async_thread.detach();

  std::vector<std::shared_ptr<SimpleBluez::Adapter>> adapters = bluez.get_adapters();
  std::shared_ptr<SimpleBluez::Adapter> adapter = adapters.at(0);

  SimpleBluez::Adapter::DiscoveryFilter filter;
  filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
  adapter->discovery_filter(filter);

  std::vector<std::shared_ptr<SimpleBluez::Device>> devices;

  adapter->set_on_device_updated([&devices, addresses](std::shared_ptr<SimpleBluez::Device> device) {
    if (addresses.count(device->name()) && std::find(devices.begin(), devices.end(), device) == devices.end()) {
      std::cout << "Detected: " << device->name() << std::endl;
      devices.push_back(device);
    }
  });

  adapter->discovery_start();
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  adapter->discovery_stop();

  std::cout << "Attempting to connect to devices" << std::endl;

  for (auto &device : devices) {
    nlohmann::json selected_device = addresses[device->name()];

    if (!adapter->powered()) {
      adapter->powered(true);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    while (!device->connected()) {
      try {
        device->connect();
      } catch (SimpleDBus::Exception::SendFailed &e) {
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }

    while (!device->services_resolved()) {
      std::cout << ".";
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    spdlog::info("Connected to {}", device->name());

    std::shared_ptr<SimpleBluez::Characteristic> characteristic;

    std::string service_uuid = selected_device["service"];
    std::string characteristic_uuid = selected_device["characteristic"];

    try {
      auto service = device->get_service(service_uuid);

      try {
        characteristic = service->get_characteristic(characteristic_uuid);
      } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
      }
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    characteristic->set_on_value_changed([&](SimpleBluez::ByteArray new_value) {
      std::string payload(new_value.begin(), new_value.end());
      std::cout << "Notified: " << payload << std::endl;
      // std::cout << "Message arrived" << std::endl;
    });

    characteristic->start_notify();

    std::cout << "Standard connection established" << std::endl;

    while (device->connected()) {
      // try
      // {
      //     characteristic->read();
      // }
      // catch (std::exception &e)
      // {
      //     std::cout << e.what() << std::endl;
      // }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    characteristic->stop_notify();
  }

  spdlog::info("Device disconnected");

  should_run = false;
  bluez_async_thread->join();

  return 0;
}
