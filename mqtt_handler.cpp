#include "mqtt_handler.hpp"
#include "de_ruyter.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

using json = nlohmann::json;

void on_connect_v5(struct mosquitto *mosq, void *obj, int reason_code, int flags [[maybe_unused]], const mosquitto_property *props [[maybe_unused]]) {
  int rc;

  DataStructure *ds = (DataStructure *)obj;

  // printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
  spdlog::info("on_connect: {}", mosquitto_connack_string(reason_code));
  if (reason_code != 0) {
    mosquitto_disconnect(mosq);
  }

  // rc = mosquitto_subscribe(mosq, NULL, "#", 1);
  // if (rc != MOSQ_ERR_SUCCESS) {
  //   spdlog::error("Error subscribing: {}", mosquitto_strerror(rc));
  //   mosquitto_disconnect(mosq);
  // }

  for (const auto &v : ds->mqtt_topics) {
    string v_str = v;
    rc = mosquitto_subscribe(mosq, NULL, v_str.c_str(), 1);
    if (rc != MOSQ_ERR_SUCCESS) {
      spdlog::error("Error subscribing to topic {}: {}", v_str, mosquitto_strerror(rc));
      // mosquitto_disconnect(mosq);
    }
  }
}

void on_subscribe_v5(struct mosquitto *mosq, void *obj [[maybe_unused]], int mid [[maybe_unused]], int qos_count, const int *granted_qos, const mosquitto_property *props [[maybe_unused]]) {
  int i;
  bool have_subscription = false;

  for (i = 0; i < qos_count; i++) {
    spdlog::info("on_subscribe: {}", i);
    spdlog::info("granted qos: {}", granted_qos[i]);
    if (granted_qos[i] <= 2) {
      have_subscription = true;
    }
  }

  if (have_subscription == false) {
    spdlog::error("Error: All subscriptions rejected");
    mosquitto_disconnect(mosq);
  }
}

void on_publish_v5(struct mosquitto *mosq [[maybe_unused]], void *obj [[maybe_unused]], int mid [[maybe_unused]], int reason_code [[maybe_unused]], const mosquitto_property *props [[maybe_unused]]) {
  // spdlog::info("Message with mid {} has been published", mid);
}

void on_message_v5(struct mosquitto *mosq [[maybe_unused]], void *obj, const struct mosquitto_message *msg, const mosquitto_property *props) {
  DataStructure *ds = (DataStructure *)obj;

  std::string payload((char *)msg->payload);
  char *n = nullptr;
  char *v = nullptr;

  mosquitto_property_read_string_pair(props, MQTT_PROP_USER_PROPERTY, &n, &v, false);

  /**
   * Fine without this, it's here to make the logs prettier
   */
  try {
    std::string payload_str((char *)msg->payload);
    json payload_json = json::parse(payload_str);
    // spdlog::warn("MQTT: Payload is JSON");

    payload_str = payload_json.dump();
  } catch (json::parse_error &e [[maybe_unused]]) {
    // spdlog::warn("MQTT: Payload is not JSON");
  }

  spdlog::info("MQTT: {} {} {}", msg->topic, msg->qos, (char *)msg->payload);

  if (props == NULL) {
    de_ruyter(ds, msg->topic, payload);
  } else {
    if (n != nullptr && v != nullptr) {
      std::string name(n), value(v);
      // spdlog::info("MQTT Props: {} {}", name, value);

      if (name != "origin" && value != "external") {
        de_ruyter(ds, msg->topic, payload);
      }
    }
  }
}

int publish_v5(struct mosquitto *mosq, string topic, string payload) { return mosquitto_publish_v5(mosq, nullptr, topic.c_str(), (int)payload.length(), payload.c_str(), 2, false, NULL); }

int publish_v5(struct mosquitto *mosq, string topic, string payload, std::pair<string, string> prop_pair) {
  int rc;
  mosquitto_property *proplist = NULL;
  rc = mosquitto_property_add_string_pair(&proplist, MQTT_PROP_USER_PROPERTY, prop_pair.first.c_str(), prop_pair.second.c_str());
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("MQTT: Cannot instantiate the proplist");
    return rc;
  }

  return mosquitto_publish_v5(mosq, nullptr, topic.c_str(), (int)payload.length(), payload.c_str(), 2, false, proplist);
}

int mqtt_handler(DataStructure *ds) {
  // struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  ds->mosq = mosquitto_new(NULL, true, ds);
  if (ds->mosq == NULL) {
    spdlog::error("Error: Out of memory");
    return 1;
  }

  mosquitto_int_option(ds->mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

  mosquitto_connect_v5_callback_set(ds->mosq, on_connect_v5);
  mosquitto_subscribe_v5_callback_set(ds->mosq, on_subscribe_v5);
  mosquitto_publish_v5_callback_set(ds->mosq, on_publish_v5);
  mosquitto_message_v5_callback_set(ds->mosq, on_message_v5);

  rc = mosquitto_connect(ds->mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(ds->mosq);
    spdlog::error("Error: {}", mosquitto_strerror(rc));
    return 1;
  }

  // mosquitto_loop_forever(dstructure->mosq, -1, 1);
  mosquitto_loop_start(ds->mosq);

  // mosquitto_lib_cleanup();

  return 0;
}
