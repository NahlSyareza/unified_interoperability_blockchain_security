#pragma once

#include "spdlog/spdlog.h"
#include <data_structure.hpp>
// #include <iostream>
// #include <map>
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include <nlohmann/json.hpp>
#include <variant>

using string = std::string;
using json = nlohmann::json;
using payload_t = std::variant<int, float, double, string, bool>;

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

void http_processor(DataStructure *dstructure, string path, string payload) {
  // dstructure->insert_map_key(&dstructure->http_map, path, payload);
  dstructure->http_map[path] = payload;
}

void mastermind(DataStructure *dstructure, json *src, json *dst, string payload, string *save) {
  string src_syntax = (*src)["format"]["syntax"];
  string dst_syntax = (*dst)["format"]["syntax"];

  // std::cout << dst_syntax << std::endl;

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

void comms_manager(DataStructure *dstructure, json *src, json *dst, string payload) {
  string src_conn = (*src)["device"]["connection"];
  string dest_conn = (*dst)["device"]["connection"];

  // spdlog::info("src: {}\ndst: {}", src->dump(2), dst->dump(2));

  string dst_name = (*dst)["name"];
  string final_payload;

  mastermind(dstructure, src, dst, payload, &final_payload);

  // spdlog::warn("Final payload: {}", final_payload);

  if (dest_conn == "wifi/http") {
    // spdlog::warn("HTTP not yet supported");
    http_processor(dstructure, dst_name, final_payload);
  } else if (dest_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, dst_name, final_payload, final_payload.length());
  } else if (dest_conn == "ble") {
    // spdlog::warn("BLE is not yet supported");
    ble_processor(dstructure, dst_name, final_payload);
  }
}

int de_ruyter(DataStructure *dstructure, string src, string payload) {
  json connection = dstructure->connection_registers[src];

  string dst = connection["destination"];

  json test_src;
  json test_dst;

  dstructure->populate(&dstructure->instance_registers, src, &test_src, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  dstructure->populate(&dstructure->instance_registers, dst, &test_dst, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  // spdlog::info("Populated src JSON is {}", test_src.dump(2));
  // spdlog::info("Populated dst JSON is {}", test_dst.dump(2));

  comms_manager(dstructure, &test_src, &test_dst, payload);

  return 0;
}
