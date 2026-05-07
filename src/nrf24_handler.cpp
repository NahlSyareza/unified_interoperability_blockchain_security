#include "nrf24_handler.hpp"

// uint8_t tx_addr[][6] = {"2Node", "6Node"};
// uint8_t rx_addr[][6] = {"1Node", "5Node"};

char incoming_payload[64] = "";

const char outgoing_payload[64] = "Internazionale Milano";

void do_receive(RF24 *radio) {
  radio->startListening();
  uint8_t pipe;
  if (radio->available(&pipe))
  {
    uint8_t bytes = radio->getPayloadSize();
    radio->read(incoming_payload, bytes);
    spdlog::info("Received {} bytes on pipe {}: {}", bytes, pipe, incoming_payload);
  }
}

void do_transmit(RF24 *radio) {
  radio->stopListening();
  bool report = radio->write(outgoing_payload, 64);

  if(report) {
    spdlog::info("Sent {}", outgoing_payload);
  }
}

int nrf24_handler(DataStructure::Instance *ds) {
  if(!ds->radio.begin()) {
    spdlog::error("Cannot initialize radio dawg");

    return 1;
  }

  ds->radio.setPALevel(RF24_PA_LOW);

  ds->radio.setPayloadSize(64);

  ds->radio.openReadingPipe(1, (uint8_t *)"1Node");

  ds->radio.stopListening((uint8_t *)"6Node");

  while (1)
  {
    do_receive(&ds->radio);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    do_transmit(&ds->radio);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

  }

  return 0;
}
