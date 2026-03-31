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
void mastermind(DataStructure *dstructure, json *src, json *dst, string payload, string *save);
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

void mastermind(DataStructure *dstructure, json *src, json *dst, string payload, string *save) {
  string src_syntax = (*src)["format"]["syntax"];
  string dst_syntax = (*dst)["format"]["syntax"];

  /**
   * Type may be NULL, since you can't really infer what a JSON's type is, unlike raw.
   */
  // string src_type = (*src)["format"]["type"];
  // string dst_type = (*dst)["format"]["type"];

  if (dst_syntax == "json") {
    json obj, temp;

    temp = (*dst)["format"];

    std::cout << temp.dump(2) << std::endl;

    if (temp.contains("pairs")) {
      temp = temp["pairs"];
      // Still extremely flawed
      for (const auto &[k, v] : temp.items()) {

        string k_v = k;
        string v_v = v;
        spdlog::warn("{} {}", k_v, v_v);

        if (v_v == "int") {
          obj[k] = std::stoi(payload);
        } else if (v_v == "float") {
          obj[k] = std::stof(payload);
        } else if (v_v == "double") {
          obj[k] = std::stod(payload);
        } else {
          obj[k] = payload;
        }
      }
    } else {
      spdlog::error("(De Ruyter) This JSON doesn't have any pairs listed!");
    }

    *save = obj.dump();

    return;
  } else {
    // This is so BAD
    *save = payload;

    return;
  }
}

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
  if (instr == "SLC") {
    int detects = std::stoi(act);
    reg->input_data = strslc(payload, detects);
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
  } else if (instr == "CONV_TO") {
    reg->convert = act;
  }

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
