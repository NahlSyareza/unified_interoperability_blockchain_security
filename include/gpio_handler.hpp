#ifndef GPIO_HANDLER_HPP
#define GPIO_HANDLER_HPP

#include "data_structure.hpp"

#define CHIP_NAME "/dev/gpiochip0"

const auto epoch_point = std::chrono::high_resolution_clock::now();

long time_now();

int gpio_handler(DataStructure::Instance* ds);

#endif
