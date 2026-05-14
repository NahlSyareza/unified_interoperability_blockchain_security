#include "uart_handler.hpp"
#include <termios.h>
#include <fcntl.h>
#include "spdlog/spdlog.h"

#define SERIAL_PATH "/dev/ttyS0"

int uart_handler(DataStructure::Instance *ds [[maybe_unused]]) {
  termios tty;
  int fd = open(SERIAL_PATH, O_RDWR | O_NONBLOCK);

  spdlog::debug("Starting UART handler...");

  if(fd < 0) {
    spdlog::error("Cannot open {}", SERIAL_PATH);
    return 1;
  }

  if(tcgetattr(fd, &tty)) {
    spdlog::error("Something wrong when getting port attributes...");
    return 1;
  }

  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  cfmakeraw(&tty);

  if(tcsetattr(fd, TCSANOW, &tty)) {
    spdlog::error("Something went wrong when SETTING port attributes...");
    return 1;
  }

  spdlog::info("UART handler up and ready!");

//  char tx_msg[64] = "The world\n";
  char rx_msg[64];

  for(int i = 0 ; i < 5 ; i++) {
//    write(fd, (uint8_t *) tx_msg, strlen(tx_msg));
    
    memset(rx_msg, 0, 64);
    read(fd, (uint8_t *) rx_msg, 64);
    
    std::cout << rx_msg << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  return 0;
}
