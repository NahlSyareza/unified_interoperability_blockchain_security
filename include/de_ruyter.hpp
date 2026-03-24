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

void comms_manager(DataStructure *dstructure, json *src, json *dest, string payload) {
  json src_device, src_format, dest_device, dest_format;

  src_device = dstructure->device_profiles[(*src)["device"]];
  src_format = dstructure->format_profiles[(*src)["format"]];

  dest_device = dstructure->device_profiles[(*dest)["device"]];
  dest_format = dstructure->format_profiles[(*dest)["format"]];

  string src_conn = src_device["connection"];
  string dest_conn = dest_device["connection"];
  string dest_name = (*dest)["name"];

  if (dest_conn == "wifi/http") {
    // spdlog::warn("HTTP not yet supported");
    http_processor(dstructure, dest_name, payload);
  } else if (dest_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, dest_name, payload, payload.length());
  } else if (dest_conn == "ble") {
    // spdlog::warn("BLE is not yet supported");
    ble_processor(dstructure, dest_name, payload);
  }
}

void mastermind(DataStructure *dstructure, string source, string payload) {
  json conn = dstructure->connection_registers[source];

  json source_device = dstructure->instance_registers[source];
  // json source_profile =
}

int de_ruyter(DataStructure *dstructure, string source, string payload) {
  json connection = dstructure->connection_registers[source];

  json src = dstructure->instance_registers[source];

  json dest = dstructure->instance_registers[connection["destination"]];

  comms_manager(dstructure, &src, &dest, payload);

  return 0;
}
