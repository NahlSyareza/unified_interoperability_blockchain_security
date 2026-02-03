#include "spdlog/spdlog.h"
#include <data_structure.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using string = std::string;
template <typename K, typename V> using map = std::map<K, V>;
using ifstream = std::ifstream;
using json = nlohmann::json;

int mqtt_processor(string topic, string payload, DataStructure *dstructure) {
  json payload_json = json::parse(payload);
  payload_json["sender"] = "gate-control";

  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, dstructure);
  if (mosq == NULL) {
    // fprintf(stderr, "Error: Out of memory.\n");
    spdlog::error("Error: Out of memory");
    return 1;
  }

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    // fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return 1;
  }

  rc = mosquitto_loop_start(mosq);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    // fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return 1;
  }

  string final_payload = payload_json.dump();
  rc = mosquitto_publish(mosq, nullptr, topic.c_str(), final_payload.length(), final_payload.c_str(), 2, false);
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

  return 0;
}

void comms_manager(DataStructure *dstructure, json *source_data, json *destination_data, string payload) {
  json deref_source = *source_data;
  json deref_destination = *destination_data;

  string deref_destination_conn = deref_destination["connection"];

  if (deref_destination_conn == "wifi/http") {
    dstructure->insert_map_key(&dstructure->http_map, deref_destination["device"], payload);
  } else if (deref_destination_conn == "wifi/mqtt") {
    mqtt_processor(deref_destination["device"], payload, dstructure);
  }
}

int de_ruyter(DataStructure *dstructure, string source, string payload) {
  ifstream connection_registers_f("./include/connection_registers.json");
  map<string, json> connection_registers_m;
  json connection_registers_j = json::parse(connection_registers_f);

  ifstream device_profiles_f("./include/device_profiles.json");
  map<string, json> device_profiles_m;
  json device_profiles_j = json::parse(device_profiles_f);

  ifstream device_registers_f("./include/device_registers.json");
  map<string, json> device_registers_m;
  json device_registers_j = json::parse(device_registers_f);

  for (const auto &item : connection_registers_j) {
    connection_registers_m[item["source"]] = item;
  }

  for (const auto &item : device_profiles_j) {
    device_profiles_m[item["name"]] = item;
  }

  for (const auto &item : device_registers_j) {
    device_registers_m[item["device"]] = item;
  }

  json ametokaze = connection_registers_m[source];

  spdlog::info("\n{}", ametokaze.dump(2));

  json komichi = device_registers_m[source];
  json hosoi = device_profiles_m[komichi["profile"]];
  hosoi["device"] = source;

  spdlog::info("\n{}", hosoi.dump(2));

  json shiro = device_registers_m[ametokaze["destination"]];
  json bara = device_profiles_m[shiro["profile"]];
  bara["device"] = ametokaze["destination"];

  spdlog::info("\n{}", bara.dump(2));

  comms_manager(dstructure, &hosoi, &bara, payload);

  return 0;
}
