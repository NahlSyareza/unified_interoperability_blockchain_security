#pragma once

#include "data_structure.hpp"
#include "mosquitto.h"
#include "mqtt_protocol.h"
#include "tour_de_scheduler.hpp"
#include "spdlog/spdlog.h"

void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
void on_subscribe(struct mosquitto *mosq, void *obj [[maybe_unused]], int mid, int qos_count, const int *granted_qos);
void on_message(struct mosquitto *mosq [[maybe_unused]], void *obj, const struct mosquitto_message *msg);
int mqtt_handler(DataStructure::Instance *dstructure);
