#pragma once

#include "data_structure.hpp"
#include "mosquitto.h"
#include "mqtt_protocol.h"

void on_connect_v5(struct mosquitto *mosq, void *obj, int reason_code, int flags, const mosquitto_property *props);
void on_subscribe_v5(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos, const mosquitto_property *props);
void on_publish_v5(struct mosquitto *mosq, void *obj, int mid, int reason_code, const mosquitto_property *props);
void on_message_v5(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg, const mosquitto_property *props);
int mqtt_handler(DataStructure *dstructure);
