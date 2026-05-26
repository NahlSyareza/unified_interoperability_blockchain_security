#include "spi_handler.hpp"
#include "ble_handler.hpp"
#include "data_structure.hpp"
#include "http_handler.hpp"
#include "mqtt_handler.hpp"
#include "rf24_handler.hpp"
#include "uart_handler.hpp"
#include "i2c_handler.hpp"
#include "gpio_handler.hpp"
#include "spi_handler.hpp"
#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include "data_structure.hpp"

int main() {
  spdlog::info("Walking by the wall");

  DataStructure::Instance ds;

  spdlog::set_level(spdlog::level::debug);

  std::thread rf24_th(rf24_handler, &ds);
  std::thread mqtt_th(mqtt_handler, &ds);
  std::thread http_th(http_handler, &ds);
  std::thread ble_th(ble_handler, &ds);
  std::thread uart_th(uart_handler, &ds);
  std::thread i2c_th(i2c_handler, &ds);
  std::thread spi_th(spi_handler, &ds);
  std::thread gpio_th(gpio_handler, &ds);

  i2c_th.join();
  rf24_th.join();
  mqtt_th.join();
  http_th.join();
  ble_th.join();
  uart_th.join();
  spi_th.join();
  gpio_th.join();

  return 0;
}
