#pragma once

#include "data_structure.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "tour_de_scheduler.hpp"
#include <iostream>

int http_handler(DataStructure::Instance *dstructure);