#include "rf24_handler.hpp"
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <spdlog/spdlog.h>
#include "data_route_handler.hpp"

char incoming_payload[64] = "";

void do_receive(DataStructure::Instance *ds) {
  ds->radio_net.update();

  if (ds->radio_net.available())
  {
    if(ds->pr_time) { 
      ds->start_time = std::chrono::high_resolution_clock::now();;
    }

    char payload[544];
    RF24NetworkHeader header1;
    ds->radio_net.read(header1, payload, sizeof(payload));

    std::string casted_payload(payload);
    ds->universal_map["rf24/011"] = casted_payload;
    data_route_handler(ds, "011");
  }
}

int rf24_handler(DataStructure::Instance *ds) {
  if(!ds->radio.begin()) {
    spdlog::error("Cannot initialize radio dawg");

    return 1;
  }

  ds->radio.setChannel(120);
  ds->radio_net.begin(011);

  while (1)
  {
    if(!ds->radio_mode)
      do_receive(ds);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
