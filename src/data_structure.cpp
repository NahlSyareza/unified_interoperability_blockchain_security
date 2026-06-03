#include "data_structure.hpp"
#include <fstream>

DataStructure::Instance::Instance() {
  /* Initialize libcoap library */
  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_WARN);

  /* Create CoAP context */
  coap_ctx = coap_new_context(nullptr);
  if (!coap_ctx) {
    coap_log_emerg("cannot initialize context\n");
  }

  /* Let libcoap do the multi-block payload handling (if any) */
  coap_context_set_block_mode(coap_ctx, COAP_BLOCK_USE_LIBCOAP|COAP_BLOCK_SINGLE_BODY);

  std::ifstream _ble_addresses("./config/ble_addresses.json");
  std::ifstream _mqtt_topics("./config/mqtt_topics.json");

  ble_addresses = nlohmann::json::parse(_ble_addresses);
  mqtt_topics = nlohmann::json::parse(_mqtt_topics);

  _mqtt_topics.close();
  _ble_addresses.close();

}
