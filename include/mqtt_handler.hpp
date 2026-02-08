#include <chrono>
#include <data_structure.hpp>
#include <de_ruyter.hpp>
#include <iostream>
#include <mosquitto.h>
// #include <mosquitto_broker.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#ifdef _WIN32
#include <io.h>
#include <process.h>
// If you were using unistd.h for sleep(), use this:
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#endif

using string = std::string;

using json = nlohmann::json;

void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
  int rc;
  // printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
  spdlog::info("on_connect: {}", mosquitto_connack_string(reason_code));
  if (reason_code != 0) {
    mosquitto_disconnect(mosq);
  }

  rc = mosquitto_subscribe(mosq, NULL, "#", 1);
  if (rc != MOSQ_ERR_SUCCESS) {
    // fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error subscribing: {}", mosquitto_strerror(rc));
    mosquitto_disconnect(mosq);
  }
}

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos) {
  int i;
  bool have_subscription = false;

  for (i = 0; i < qos_count; i++) {
    // printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
    spdlog::info("on_subscribe: {}", i);
    spdlog::info("granted qos: {}", granted_qos[i]);
    if (granted_qos[i] <= 2) {
      have_subscription = true;
    }
  }

  if (have_subscription == false) {
    fprintf(stderr, "Error: All subscriptions rejected.\n");
    spdlog::error("Error: All subscriptions rejected");
    mosquitto_disconnect(mosq);
  }
}

void on_publish(struct mosquitto *mosq, void *obj, int mid) {
  // printf("Message with mid %d has been published.\n", mid);
  spdlog::info("Message with mid {} has been published", mid);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
  DataStructure *dstructure = (DataStructure *)obj;

  // printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
  spdlog::info("{} {} {}", msg->topic, msg->qos, (char *)msg->payload);

  json payload;
  try {
    payload = json::parse((char *)msg->payload);
  } catch (json::parse_error &e) {
    spdlog::error("{}", e.what());
    return;
  }
  /**
   * on_message also triggers when this code publishes message
   */

  if (payload["sender"] != "gate-control") {
    de_ruyter(dstructure, msg->topic, (char *)msg->payload);
  }
}

int mqtt_handler(DataStructure *dstructure) {
  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, dstructure);
  if (mosq == NULL) {
    // fprintf(stderr, "Error: Out of memory.\n");
    spdlog::error("Error: Out of memory");
    return 1;
  }

  mosquitto_message_callback_set(mosq, on_message);
  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_subscribe_callback_set(mosq, on_subscribe);
  mosquitto_publish_callback_set(mosq, on_publish);

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    // fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return 1;
  }

  mosquitto_loop_forever(mosq, -1, 1);

  mosquitto_lib_cleanup();

  return 0;
}
