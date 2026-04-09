#pragma once

#include "data_structure.hpp"
#include "nlohmann/json.hpp"
#include "spitarm.hpp"

int de_ruyter(DataStructure *dstructure, string src, string payload);
void comms_manager(DataStructure *dstructure, json *src, json *dst, string payload);
void process_instr(string instr, string act, string payload, OperationRegister *reg);
void get_instr(string op, string data, OperationRegister *reg);
void http_processor(DataStructure *dstructure, string path, string payload);
void mqtt_processor(DataStructure *dstructure, string topic, string payload);
void ble_processor(DataStructure *dstructure, string identifier, string payload);
