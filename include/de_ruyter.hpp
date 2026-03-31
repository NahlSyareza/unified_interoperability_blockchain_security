#pragma once

#include "data_structure.hpp"
#include "mosquitto.h"
#include "mqtt_protocol.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "spitarm.hpp"
#include <fstream>
#include <sstream>

using string = std::string;
using json = nlohmann::json;

int de_ruyter(DataStructure *dstructure, string src, string payload);
void comms_manager(DataStructure *dstructure, json *src, json *dst, string payload);
void process_instr(string instr, string act, string payload, OperationRegister *reg);
void get_instr(string op, string data, OperationRegister *reg);
void http_processor(DataStructure *dstructure, string path, string payload);
void mqtt_processor(DataStructure *dstructure, string topic, string payload, int payloadlen);
void ble_processor(DataStructure *dstructure, std::string identifier, string payload);

void ble_processor(DataStructure *dstructure, std::string identifier, string payload) {
  for (auto &p : dstructure->peripherals) {
    if (p.identifier() == identifier)
      p.write_request(dstructure->uuid_pair[identifier].first, dstructure->uuid_pair[identifier].second, payload);
  }
}

void mqtt_processor(DataStructure *dstructure, string topic, string payload, int payloadlen) {
  int rc;

  mosquitto_property *proplist = NULL;
  rc = mosquitto_property_add_string_pair(&proplist, MQTT_PROP_USER_PROPERTY, "origin", "external");
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Something's wrong I can feel it");
  }

  string final_payload = payload;
  rc = mosquitto_publish_v5(dstructure->mosq, nullptr, topic.c_str(), payloadlen, payload.c_str(), 2, false, proplist);
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  }
}

void http_processor(DataStructure *dstructure, string path, string payload) { dstructure->http_map[path] = payload; }

void get_instr(string op, string payload, OperationRegister *reg) {
  int ctrl;
  string instr, act;

  // Perhaps find a way to detect a non-space first character
  if (op.at(0) == ';') {
    return;
  }

  for (ctrl = 0; ctrl < op.length(); ctrl++) {
    if (op.at(ctrl) == ' ' || op.at(ctrl) == '\n') {
      break;
    }
  }

  instr.assign(op, 0, ctrl);

  if (ctrl + 1 < op.length()) {
    act.assign(op, ctrl + 1, op.length() - ctrl + 1);
  } else {
    // std::cout << "Illegal operation attempted. Maybe caused by incomplete op." << std::endl;
  }

  // std::cout << "Instruction: " << instr << std::endl;
  // std::cout << "Act: " << act << std::endl;

  process_instr(instr, act, payload, reg);
}

void process_instr(string instr, string act, string payload, OperationRegister *reg) {
  spdlog::debug("Running instruction: {}", instr);

  if (instr == "GET") {
    int detects = std::stoi(act);
    reg->input_data = strget(payload, detects);
  } else if (instr == "GET_FROM") {
    json json_obj;

    try {
      json_obj = json::parse(payload);
      if (!json_obj.is_object()) {
        // This shouldn't happen to be honest
        throw json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const json::parse_error &e) {
      spdlog::error("(De Ruyter) This is NOT supposed to happen");
    }

    reg->input_data = json_obj[act].dump();
  } else if (instr == "TYPE") {
    reg->type = act;
  } else if (instr == "ASGN_TO") {
    json json_obj;

    try {
      json_obj = json::parse(reg->output_data);
      if (!json_obj.is_object()) {
        throw json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const json::parse_error &e) {
      // std::cout << e.what() << std::endl;
      json_obj = json::parse("{}");
    }

    try {
      if (reg->convert == "int") {
        json_obj[act] = std::stoi(reg->input_data);
      } else if (reg->convert == "double") {
        json_obj[act] = std::stod(reg->input_data);
      } else {
        json_obj[act] = reg->input_data;
      }
    } catch (const std::invalid_argument &e) {
      // std::cout << e.what() << std::endl;
      json_obj[act] = reg->input_data;
    }

    reg->convert = "";
    reg->output_data = json_obj.dump();
  } else if (instr == "CLCT_FROM") {
    json json_obj;

    try {
      json_obj = json::parse(payload);
      if (!json_obj.is_object()) {
        // This shouldn't happen to be honest
        throw json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const json::parse_error &e) {
      spdlog::error("(De Ruyter) This is NOT supposed to happen");
    }

    string deref = json_obj[act];

    reg->output_data = deref;
  } else if (instr == "IF") {
    // spdlog::debug("(Process Instr) Comparing {} with {}", act, reg->input_data);
    string compared_value = strget(act, 1);

    spdlog::debug("This is the actual value to be compared: {}", compared_value);
    // reg->logic_comparison = act == reg->input_data;

    reg->logic_comparison = compare(act, std::stoi(reg->input_data), std::stoi(compared_value));
  } else if (instr == "CONV_TO") {
    reg->convert = act;
  } else if (instr == "THEN") {
    if (reg->logic_comparison)
      reg->output_data = act;
  } else if (instr == "OTHERWISE") {
    if (!reg->logic_comparison)
      reg->output_data = act;
  } else {
    spdlog::error("(Process Instr) Unrecognized instruction {}", instr);
  }

  // print_op_reg(reg);
  // std::cout << "Reg.type: " << reg->type << " Reg.input_data: " << reg->input_data << " Reg.output_data: " << reg->output_data << " Reg.convert: " << reg->convert << std::endl << std::endl;
}

void comms_manager(DataStructure *dstructure, json *src, json *dst, string payload, string rule_file_location) {
  string src_conn = (*src)["device"]["connection"];
  string dest_conn = (*dst)["device"]["connection"];
  std::ifstream rule_file("./rules/" + rule_file_location);
  std::stringstream ss;

  // spdlog::info("src: {}\ndst: {}", src->dump(2), dst->dump(2));

  string dst_name = (*dst)["name"];

  OperationRegister op_reg;

  string line;

  if (rule_file) {
    ss << rule_file.rdbuf();
    rule_file.close();

    while (std::getline(ss, line)) {
      get_instr(line, payload, &op_reg);
    }

    spdlog::warn("(OperationRegister) {}", op_reg.output_data);
  } else {
    spdlog::error("(De Ruyter) Rule file location not found? Perhaps a typo? Or maybe deliberate.");
  }

  // mastermind(dstructure, src, dst, payload, &final_payload);

  // spdlog::warn("Final payload: {}", final_payload);

  string final_payload = !op_reg.output_data.empty() ? op_reg.output_data : payload;

  if (dest_conn == "wifi/http") {
    http_processor(dstructure, dst_name, final_payload);
  } else if (dest_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, dst_name, final_payload, final_payload.length());
  } else if (dest_conn == "ble") {
    ble_processor(dstructure, dst_name, final_payload);
  }
}

int de_ruyter(DataStructure *dstructure, string src, string payload) {
  json connection = dstructure->connection_registers[src];

  string dst = connection["destination"];

  string rules = "";
  if (connection.count("rules") > 0) {
    rules = connection["rules"];
  }

  json test_src;
  json test_dst;

  dstructure->populate(&dstructure->instance_registers, src, &test_src, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  dstructure->populate(&dstructure->instance_registers, dst, &test_dst, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  comms_manager(dstructure, &test_src, &test_dst, payload, rules);

  return 0;
}
