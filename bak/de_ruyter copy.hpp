#include "spdlog/spdlog.h"
#include <data_structure.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <nlohmann/json.hpp>

using string = std::string;
template <typename K, typename V> using map = std::map<K, V>;
using ifstream = std::ifstream;
using json = nlohmann::json;

struct Contents {
  string source;
  string destination;
};

int de_ruyter(DataStructure *dstructure, string source, string payload) {
  ifstream connection_registers_f("./include/connection_registers.json");
  map<string, Contents *> connection_register_m;
  json connection_register_j = json::parse(connection_registers_f);

  json connections_file = json::parse(f);

  for (const auto &item : connections_file) {
    string source = item["source"];
    string destination = item["destination"];

    connection_register[source] = (Contents *)calloc(1, sizeof(Contents));
    new (&connection_register[source]->destination) string();

    connection_register[source]->destination = destination;
  }

  Contents *selected_contents = connection_register[source];

  if (selected_contents->protocol == "mqtt") {
    mqtt_processor();
    // dstructure->insert_map_key(&dstructure->mqtt_map, selected_contents->destination, payload);
  } else if (selected_contents->protocol == "http") {
    dstructure->insert_map_key(&dstructure->http_map, selected_contents->destination, payload);
  }

  return 0;
}

void mqtt_processor() {
  json payload_json = json::parse(payload);
  payload_json["sender"] = "gate-control";

  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, dstructure);
  if (mosq == NULL) {
    fprintf(stderr, "Error: Out of memory.\n");
    return 1;
  }

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    return 1;
  }

  rc = mosquitto_loop_start(mosq);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    return 1;
  }

  string final_payload = payload_json.dump();
  rc = mosquitto_publish(mosq, nullptr, selected_contents->destination.c_str(), final_payload.length(), final_payload.c_str(), 2, false);
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
}