#pragma once

#include "RF24/RF24.h"
#include "data_structure.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

int nrf24_handler(DataStructure::Instance *ds);
