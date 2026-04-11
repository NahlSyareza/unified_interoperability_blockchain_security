#pragma once

#include "data_structure.hpp"
#include "nlohmann/json.hpp"
#include "spitarm.hpp"

int de_ruyter(DataStructure *dstructure, std::string src, std::string payload);
void comms_manager(DataStructure *dstructure, nlohmann::json *src, nlohmann::json *dst, std::string payload, std::string rule_file_location);
void process_instr(std::string instr, std::string act, std::string payload, OperationRegister *reg);
void get_instr(std::string op, std::string data, OperationRegister *reg);
void http_processor(DataStructure *dstructure, std::string path, std::string payload);
void mqtt_processor(DataStructure *dstructure, std::string topic, std::string payload);
void ble_processor(DataStructure *dstructure, std::string identifier, std::string payload);
