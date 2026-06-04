#include "rf24_handler.hpp"
#include <RF24/RF24.h>
#include <spdlog/spdlog.h>
#include "data_route_handler.hpp"

char incoming_payload[64] = "";

void do_receive(DataStructure::Instance *ds) {
  ds->radio.startListening();
  uint8_t pipe;
  if (ds->radio.available(&pipe))
  {
    uint8_t bytes = ds->radio.getPayloadSize();
    ds->radio.read(incoming_payload, bytes);

    if(ds->pr_time) { 
      auto current_point = std::chrono::high_resolution_clock::now();
      auto dur = std::chrono::duration_cast<std::chrono::microseconds>(current_point - ds->epoch_point);
      ds->start_time = dur.count();
    }

    ds->universal_map["rf24/1Node"] = incoming_payload;
    data_route_handler(ds, "1Node");
  }
}

int rf24_handler(DataStructure::Instance *ds) {
  if(!ds->radio.begin()) {
    spdlog::error("Cannot initialize radio dawg");

    return 1;
  }

  ds->radio.setPALevel(RF24_PA_LOW);

  ds->radio.setPayloadSize(64);

  ds->radio.openReadingPipe(1, (uint8_t *)"1Node");

  ds->radio.stopListening((uint8_t *)"2Node");

  while (1)
  {
    if(!ds->radio_mode)
      do_receive(ds);
  }

  return 0;
}
