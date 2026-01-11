#include <iostream>
#include <string>
#include <mosquitto.h>

// --- YOUR CUSTOM LOGIC AREA ---
// This is the only function you'll need to edit for your thesis.
void apply_custom_logic(struct mosquitto *mosq, std::string topic, std::string payload)
{
  std::cout << "\n[LOGIC] Intercepted Topic: " << topic << std::endl;
  std::cout << "[LOGIC] Original Data: " << payload << std::endl;

  // BASIC EXAMPLE: Transform data and re-route it
  if (topic == "sensors/raw")
  {
    std::string processed = "VERIFIED_BY_GATEWAY: " + payload;

    // Forward the "Safe" data to a new topic
    mosquitto_publish(mosq, NULL, "sensors/verified", processed.length(), processed.c_str(), 0, false);

    std::cout << "[ACTION] Logic applied. Data forwarded to 'sensors/verified'" << std::endl;
  }
}

// --- MQTT CALLBACKS (The Plumbing) ---
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
  if (rc == 0)
  {
    std::cout << "SUCCESS: Connected to Broker!" << std::endl;
    // Subscribe to everything to act as a gateway/middleman
    mosquitto_subscribe(mosq, NULL, "#", 0);
  }
  else
  {
    std::cerr << "FAILED: Connection error code " << rc << std::endl;
  }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
  if (msg->payload)
  {
    // Convert raw payload to C++ string for easier handling
    std::string topic = msg->topic;
    std::string payload = std::string((char *)msg->payload, msg->payloadlen);

    apply_custom_logic(mosq, topic, payload);
  }
}

// --- MAIN RUNNER ---
int main()
{
  // Required for Windows networking
  mosquitto_lib_init();

  struct mosquitto *mosq = mosquitto_new("gate-control-1", true, NULL);

  // Wire up the callbacks
  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_message_callback_set(mosq, on_message);

  // Connect (Change 127.0.0.1 to your Pi's IP if running remotely)
  if (mosquitto_connect(mosq, "127.0.0.1", 1883, 60) != MOSQ_ERR_SUCCESS)
  {
    std::cerr << "Critical: Could not find MQTT Broker." << std::endl;
    return 1;
  }

  std::cout << "Gateway operational. Watching all topics..." << std::endl;

  // Starts the background loop (non-blocking version)
  mosquitto_loop_forever(mosq, -1, 1);

  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
  return 0;
}