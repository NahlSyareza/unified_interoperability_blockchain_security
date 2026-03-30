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
  // int rc = publish_v5(dstructure->mosq, topic, payload, std::make_pair("origin", "external"));

  // if (rc != MOSQ_ERR_SUCCESS) {
  //   spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  // }

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

void comms_manager(DataStructure *dstructure, json *source, json *destination, string payload) {
  string src_conn = (*source)["device"]["connection"];
  string dest_conn = (*destination)["device"]["connection"];

  string dst = (*destination)["name"];

  if (dest_conn == "wifi/http") {
    // spdlog::warn("HTTP not yet supported");
    http_processor(dstructure, dst, payload);
  } else if (dest_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, dst, payload, payload.length());
  } else if (dest_conn == "ble") {
    // spdlog::warn("BLE is not yet supported");
    ble_processor(dstructure, dst, payload);
  }
}

void mastermind(DataStructure *dstructure, string source, string payload) {
  json conn = dstructure->connection_registers[source];

  json source_device = dstructure->instance_registers[source];
  // json source_profile =
}

int de_ruyter(DataStructure *dstructure, string source, string payload) {
  json connection = dstructure->connection_registers[source];

  string destination = connection["destination"];

  json test_src;
  json test_dst;

  dstructure->populate(&dstructure->instance_registers, source, &test_src, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  dstructure->populate(&dstructure->instance_registers, destination, &test_dst, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  // spdlog::info("Populated src JSON is {}", test_src.dump(2));
  // spdlog::info("Populated dst JSON is {}", test_dst.dump(2));

  comms_manager(dstructure, &test_src, &test_dst, payload);

  return 0;
}
