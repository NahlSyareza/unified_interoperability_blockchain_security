#include "ble_handler.hpp"
#include "data_structure.hpp"
#include "http_handler.hpp"
#include "mqtt_handler.hpp"
#include "rf24_handler.hpp"
#include "uart_handler.hpp"
#include "i2c_handler.hpp"
#include "gpio_handler.hpp"
#include <iostream>
#include <thread>

int main() {
  spdlog::info("Walking by the wall");

  DataStructure::Instance ds;

  spdlog::set_level(spdlog::level::debug);

//  std::thread rf24_thread(rf24_handler, &ds);
//  std::thread mqtt_thread(mqtt_handler, &ds);
//  std::thread http_thread(http_handler, &ds);
//  std::thread ble_thread(ble_handler, &ds);
//  std::thread uart_thread(uart_handler, &ds);
//  std::thread i2c_thread(i2c_handler, &ds);

//  i2c_thread.join();
//  rf24_thread.join();
//  mqtt_thread.join();
//  http_thread.join();
//  ble_thread.join();
//  uart_thread.join();

  std::thread wp_thread(gpio_handler, &ds);

  wp_thread.join();

  return 0;
}
