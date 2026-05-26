#include <spi_handler.hpp>
#include <spdlog/spdlog.h>

#define BUFFER_SIZE 128

uint8_t tx_bf[BUFFER_SIZE] = "haramball no los entenderias";
uint8_t rx_bf[BUFFER_SIZE] = {0};

int spi_handler(DataStructure::Instance* ds) {
  if(ds->spi_h.begin()) {
    spdlog::info("Trying to send {}", (char*)tx_bf);
    ds->spi_h.xfer(tx_bf, BUFFER_SIZE, rx_bf, BUFFER_SIZE);
    spdlog::info("Retrieved {}", (char*)rx_bf);
  }

  return 0;
}
