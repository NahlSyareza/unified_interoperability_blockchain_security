#pragma once

#include "data_structure.hpp"
#include "mosquitto.h"
#include "mqtt_protocol.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spitarm.hpp"
#include <fstream>
#include <sstream>

void de_ruyter(DataStructure::Instance *ds, nlohmann::json *interop_data, std::string rule_file_location);
void process_instr(std::string instr, std::string act, std::string payload, OperationRegister *reg);
void get_instr(std::string op, std::string data, OperationRegister *reg);
void http_processor(DataStructure::Instance *dstructure, std::string path, std::string payload);
void mqtt_processor(DataStructure::Instance *dstructure, std::string topic, std::string payload);
void ble_processor(DataStructure::Instance *dstructure, std::string identifier, std::string payload);
