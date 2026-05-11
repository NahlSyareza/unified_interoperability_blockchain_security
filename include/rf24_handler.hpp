#pragma once

#include "RF24/RF24.h"
#include "data_structure.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include "tour_de_scheduler.hpp"

int rf24_handler(DataStructure::Instance *ds);
void do_receive(DataStructure::Instance *ds);
void do_transmit(DataStructure::Instance *ds);
