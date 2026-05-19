extern "C" {
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>
#include <cstdint>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "i2c_handler.hpp"
#include "tour_de_scheduler.hpp"

#define PORT_PATH "/dev/i2c-1"

int i2c_handler(DataStructure::Instance* ds) {
  ds->i2c_fd = open(PORT_PATH, O_RDWR);

  if (ds->i2c_fd < 0) {
    std::cerr << "Cannot open file" << std::endl;
    return 1;
  }

  int addr = 0x67;

  if(ioctl(ds->i2c_fd, I2C_SLAVE, addr) < 0) {
    std::cerr << "Pizdec" << std::endl;
    return 1;
  }

  uint8_t i_bf[64];

  ssize_t starter_read = read(ds->i2c_fd, i_bf, sizeof(i_bf));

  if(starter_read < 0) {
    spdlog::error("I2C not connected");
    return 1;
  }

  spdlog::info("I2C connected!");

  while(1) {
    memset(i_bf, 0, sizeof(i_bf));
    ssize_t bytes_received [[maybe_unused]] = read(ds->i2c_fd, i_bf, sizeof(i_bf));
    
    if(*(i_bf)) {
      std::string payload((char *) i_bf);
      ds->universal_map["i2c/i2cSen"] = payload;

      if(!ds->active_registers.count("i2cSen")) {
        create_task_detached(ds, "i2cSen");
      }
    }
  }

  return 0;
}

