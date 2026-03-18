#pragma once

#include "spdlog/spdlog.h"
#include <data_structure.hpp>
// #include <iostream>
// #include <map>
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include <nlohmann/json.hpp>

using string = std::string;
using json = nlohmann::json;

void ble_processor(DataStructure *dstructure, std::string identifier, string payload) {
  for (auto &p : dstructure->peripherals) {
    if (p.identifier() == identifier)
      p.write_request(dstructure->uuid_pair[identifier].first, dstructure->uuid_pair[identifier].second, payload);
  }
}

void mqtt_processor(DataStructure *dstructure, string topic, string payload) {
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
  rc = mosquitto_publish_v5(dstructure->mosq, nullptr, topic.c_str(), final_payload.length(), final_payload.c_str(), 2, false, proplist);
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  }
}

void http_processor(DataStructure *dstructure, string path, string payload) {
  // dstructure->insert_map_key(&dstructure->http_map, path, payload);
  dstructure->http_map[path] = payload;
}

void comms_manager(DataStructure *dstructure, json *source_data, json *destination_data, string payload) {
  json deref_source = *source_data;
  json deref_destination = *destination_data;

  string deref_destination_conn = deref_destination["connection"];

  if (deref_destination_conn == "wifi/http") {
    http_processor(dstructure, deref_destination["device"], payload);
  } else if (deref_destination_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, deref_destination["device"], payload);
  } else if (deref_destination_conn == "ble") {
    ble_processor(dstructure, deref_destination["device"], payload);
  }
}

void mastermind(DataStructure *dstructure, string source, string payload) {
  json conn = dstructure->connection_registers[source];

  json source_device = dstructure->device_registers[source];
  // json source_profile =
}

int de_ruyter(DataStructure *dstructure, string source, string payload) {
  json ametokaze = dstructure->connection_registers[source];

  // spdlog::info("\n{}", ametokaze.dump(2));

  json komichi = dstructure->device_registers[source];
  json hosoi = dstructure->device_profiles[komichi["profile"]];
  hosoi["device"] = source;

  // spdlog::info("\n{}", hosoi.dump(2));

  json shiro = dstructure->device_registers[ametokaze["destination"]];
  json bara = dstructure->device_profiles[shiro["profile"]];
  bara["device"] = ametokaze["destination"];

  // spdlog::info("\n{}", bara.dump(2));

  // spdlog::info("{}", payload);

  comms_manager(dstructure, &hosoi, &bara, payload);

  return 0;
}
