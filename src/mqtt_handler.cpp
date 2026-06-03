#include "mqtt_handler.hpp"
#include <mqtt_protocol.h>
#include "data_route_handler.hpp"
#include <spdlog/spdlog.h>

void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
  int rc;

  DataStructure::Instance *ds = (DataStructure::Instance *)obj;

  spdlog::info("on_connect: {}", mosquitto_connack_string(reason_code));
  if (reason_code != 0) {
    mosquitto_disconnect(mosq);
  }

  for (const auto &v : ds->mqtt_topics) {
    std::string v_str = v;
    rc = mosquitto_subscribe(mosq, NULL, v_str.c_str(), 1);
    if (rc != MOSQ_ERR_SUCCESS) {
      spdlog::error("Error subscribing to topic {}: {}", v_str, mosquitto_strerror(rc));
      // mosquitto_disconnect(mosq);
    }
  }
}

void on_subscribe(struct mosquitto *mosq, void *obj [[maybe_unused]], int mid, int qos_count, const int *granted_qos) {
  int i;
  bool have_subscription = false;

  // DataStructure *ds = (DataStructure *)obj;

  for (i = 0; i < qos_count; i++) {
    spdlog::info("on_subscribe: {} qos: {} mid: {}", i, granted_qos[i], mid);
    if (granted_qos[i] <= 2) {
      have_subscription = true;
    }
  }

  if (have_subscription == false) {
    spdlog::error("Error: All subscriptions rejected");
    mosquitto_disconnect(mosq);
  }
}

void on_message(struct mosquitto *mosq [[maybe_unused]], void *obj, const struct mosquitto_message *msg) {
  DataStructure::Instance *ds = (DataStructure::Instance *)obj;

  // spdlog::debug("MQTT: {} {} {}", msg->topic, msg->qos, (char *)msg->payload);

  if(ds->pr_time) {
    auto current_point = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(current_point - ds->epoch_point);
    spdlog::info("Start: {}", dur.count());
  }

  std::string topic((char *)msg->topic);
  std::string payload((char *)msg->payload);

  ds->universal_map["mqtt/" + topic] = payload;

  data_route_handler(ds, topic);

  // if (!ds->active_registers.count(topic)) {
  //   create_task_detached(ds, topic);
  // }
}

int mqtt_handler(DataStructure::Instance *ds) {
  int rc;

  mosquitto_lib_init();

  ds->mosq = mosquitto_new(NULL, true, (void *)ds);
  if (ds->mosq == NULL) {
    spdlog::error("Error: Out of memory");
    return 1;
  }

  // mosquitto_int_option(ds->mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

  mosquitto_connect_callback_set(ds->mosq, on_connect);
  mosquitto_subscribe_callback_set(ds->mosq, on_subscribe);
  mosquitto_message_callback_set(ds->mosq, on_message);

  rc = mosquitto_connect(ds->mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(ds->mosq);
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return 1;
  }

  spdlog::info("MQTT initialized");
  mosquitto_loop_forever(ds->mosq, -1, 1);
  // mosquitto_loop_start(ds->mosq);

  mosquitto_lib_cleanup();

  return 0;
}
