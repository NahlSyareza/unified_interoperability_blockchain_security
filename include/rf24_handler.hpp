#pragma once

#include "data_structure.hpp"

int rf24_handler(DataStructure::Instance *ds);
void do_receive(DataStructure::Instance *ds);
void do_transmit(DataStructure::Instance *ds);
