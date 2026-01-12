#include <iostream>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hitotsu.hpp>

using namespace std;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
  Fucker *f = (Fucker *)obj;
  if (f != NULL)
  {
    printf("Dekinai %d\n", f->superbialis);
  }

  printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int mqtt_handler(Fucker *azula)
{
  struct mosquitto *mosq;
  int rc;

  mosquitto_lib_init();

  mosq = mosquitto_new(NULL, true, azula);
  if (mosq == NULL)
  {
    fprintf(stderr, "Error: Out of memory.\n");
    return 1;
  }

  mosquitto_message_callback_set(mosq, on_message);

  rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
  if (rc != MOSQ_ERR_SUCCESS)
  {
    mosquitto_destroy(mosq);
    fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
    return 1;
  }

  rc = mosquitto_subscribe(mosq, NULL, "#", 1);
  if (rc != MOSQ_ERR_SUCCESS)
  {
    fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
    mosquitto_disconnect(mosq);
  }

  mosquitto_loop_forever(mosq, -1, 1);

  mosquitto_lib_cleanup();

  return 0;
}
