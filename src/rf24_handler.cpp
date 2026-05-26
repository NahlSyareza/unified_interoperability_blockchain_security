#include "rf24_handler.hpp"
#include <RF24/RF24.h>
#include <spdlog/spdlog.h>
#include "task_scheduler.hpp"

char incoming_payload[64] = "";

const char outgoing_payload[64] = "Internazionale Milano";

void do_receive(DataStructure::Instance *ds) {
  ds->radio.startListening();
  uint8_t pipe;
  if (ds->radio.available(&pipe))
  {
    uint8_t bytes = ds->radio.getPayloadSize();
    ds->radio.read(incoming_payload, bytes);
    // spdlog::debug("Received {} bytes on pipe {}: {}", bytes, pipe, incoming_payload);

    ds->rx_rf24_map["1Node"] = incoming_payload;

    if(!ds->active_registers.count("1Node"))
      create_task_detached(ds, "1Node");
  }
}

void do_transmit(DataStructure::Instance* ds) {
  ds->radio.stopListening();

  std::string ot_payload = "";

  if(ds->tx_rf24_map.count("2Node")) {
    spdlog::debug("(RF24) payload is available");
    ot_payload = ds->tx_rf24_map["2Node"];

    bool report = ds->radio.write(ot_payload.c_str(), 64);

    if(report) {
      spdlog::debug("Sent {}", outgoing_payload);
    }
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
    do_receive(ds);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    do_transmit(ds);
    //    ds->radio.stopListening();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

  }

  return 0;
}
