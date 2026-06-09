#pragma once

#include "data_structure.hpp"
#include "nlohmann/json.hpp"
#include "cust_instr_handler.hpp"

void data_route_handler(DataStructure::Instance *ds, std::string source);
void process_instr(std::string instr, std::string act, std::string payload, OperationRegister *reg);
void get_instr(std::string op, std::string data, OperationRegister *reg);
void http_processor(DataStructure::Instance *dstructure, std::string path, std::string payload);
void mqtt_processor(DataStructure::Instance *dstructure, std::string topic, std::string payload);
void ble_processor(DataStructure::Instance *dstructure, std::string identifier, std::string payload);
void uart_processor(DataStructure::Instance *ds, std::string payload);
void i2c_processor(DataStructure::Instance *ds, std::string payload);
void extract_config(std::string path, nlohmann::json *json_ptr);
bool create_interop_data(DataStructure::Instance *ds, std::string source, nlohmann::json *json_ptr);
