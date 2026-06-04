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

void DataStructure::Instance::save_pr_time() {
  if(pr_time_count < max_pr_time_count) {
    pr_time_detailed.push_back(std::make_pair(start_time, end_time));
    pr_time_sum.push_back(end_time - start_time);
  } else {
    fprintf(stdout, "Measured in microseconds");
    int index = 0;
    for(auto& e : pr_time_sum) {
      fprintf(stdout, "(%d) Time: %ld\n", index, e);
      index++;
    }     

    for(auto& e : pr_time_detailed) {
      fprintf(stdout, "%ld %ld\n", e.first, e.second);
    }

    exit(0);
  }

  pr_time_count++;
}
