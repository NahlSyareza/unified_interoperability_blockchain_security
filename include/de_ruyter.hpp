#pragma once

#include "spdlog/spdlog.h"
#include <data_structure.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include <nlohmann/json.hpp>

// using std::cout;
// using std::endl;
using string = std::string;
using ifstream = std::ifstream;
using json = nlohmann::json;

void ble_processor(DataStructure *dstructure, std::string identifier, string payload) {
  for (auto &p : dstructure->peripherals) {
    if (p.identifier() == identifier)
      p.write_request(dstructure->uuid_pair[identifier].first, dstructure->uuid_pair[identifier].second, payload);
  }
}

void mqtt_processor(DataStructure *dstructure, string topic, string payload) {
  // json payload_json = json::parse(payload);
  // payload_json["sender"] = "gate-control";

  mosquitto_property *proplist = NULL;
  mosquitto_property_add_string_pair(&proplist, MQTT_PROP_USER_PROPERTY, "origin", "external");

  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, dstructure);
  if (mosq == NULL) {
    // fprintf(stderr, "Error: Out of memory.\n");
    spdlog::error("Error: Out of memory");
    return;
  }

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    // fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return;
  }

  rc = mosquitto_loop_start(mosq);
  rc = mosquitto_loop(mosq, 1000, 5);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    // fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return;
  }

  string final_payload = payload;
  /**
   * arr stands for already re routed
   */
  final_payload.insert(0, "arr");
  rc = mosquitto_publish(mosq, nullptr, topic.c_str(), final_payload.length(), final_payload.c_str(), 2, false);
  // rc = mosquitto_publish_v5(mosq, nullptr, topic.c_str(), final_payload.length(), final_payload.c_str(), 2, false, proplist);
  if (rc != MOSQ_ERR_SUCCESS) {
    // fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  }

  int timeout = 0;

  while (timeout < 5) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timeout++;
  }

  mosquitto_disconnect(mosq);
  mosquitto_loop_stop(mosq, false);
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();

  // return 0;
}

void http_processor(DataStructure *dstructure, string path, string payload) { dstructure->insert_map_key(&dstructure->http_map, path, payload); }

void comms_manager(DataStructure *dstructure, json *source_data, json *destination_data, string payload) {
  json deref_source = *source_data;
  json deref_destination = *destination_data;

  // spdlog::info("Source:\n{}", deref_source.dump(2));
  // spdlog::info("Destination:\n{}", deref_destination.dump(2));

  string deref_destination_conn = deref_destination["connection"];

  // spdlog::info("Source format:\n{}", dstructure->format_profiles_m[deref_source["format"]].dump(2));
  // spdlog::info("Destination format:\n{}", dstructure->format_profiles_m[deref_destination["format"]].dump(2));

  if (deref_destination_conn == "wifi/http") {
    // dstructure->insert_map_key(&dstructure->http_map, deref_destination["device"], payload);
    http_processor(dstructure, deref_destination["device"], payload);
  } else if (deref_destination_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, deref_destination["device"], payload);
  } else if (deref_destination_conn == "ble") {
    ble_processor(dstructure, deref_destination["device"], payload);
  }
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
