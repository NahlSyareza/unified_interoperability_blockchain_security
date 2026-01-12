#include <data_queues.hpp>
#include <iostream>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
  int rc;
  printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
  if (reason_code != 0) {
    mosquitto_disconnect(mosq);
  }

  rc = mosquitto_subscribe(mosq, NULL, "#", 1);
  if (rc != MOSQ_ERR_SUCCESS) {
    fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
    mosquitto_disconnect(mosq);
  }
}

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos) {
  int i;
  bool have_subscription = false;

  for (i = 0; i < qos_count; i++) {
    printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
    if (granted_qos[i] <= 2) {
      have_subscription = true;
    }
  }

  if (have_subscription == false) {
    fprintf(stderr, "Error: All subscriptions rejected.\n");
    mosquitto_disconnect(mosq);
  }
}

// void on_publish

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
  Queues *ques = (Queues *)obj;

  Queues::Node *current = ques->pop_tropica(ques->mqttlist);

  if (current != NULL) {
    json payload = json::parse(current->payload);

    cout << payload << endl;
  } else {
    cout << "There is no payload!" << endl;
  }

  // ques->print_linked_list(ques->mqttlist);

  printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int mqtt_handler(Queues *ques) {
  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, ques);
  if (mosq == NULL) {
    fprintf(stderr, "Error: Out of memory.\n");
    return 1;
  }

  mosquitto_message_callback_set(mosq, on_message);
  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_subscribe_callback_set(mosq, on_subscribe);

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    mosquitto_destroy(mosq);
    fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    return 1;
  }

  mosquitto_loop_forever(mosq, -1, 1);

  mosquitto_lib_cleanup();

  return 0;
}
