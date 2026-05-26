#include "uart_handler.hpp"
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include "task_scheduler.hpp"

#define SERIAL_PATH "/dev/ttyS0"

int uart_handler(DataStructure::Instance *ds) {
  // termios tty;
  ds->uart_h = open(SERIAL_PATH, O_RDWR);

  if(ds->uart_h < 0) {
    spdlog::error("Cannot open {}", SERIAL_PATH);
    return 1;
  }

  if(tcgetattr(ds->uart_h, &ds->tty)) {
    spdlog::error("Something wrong when getting port attributes...");
    return 1;
  }

  ds->tty.c_cflag &= ~PARENB;
  ds->tty.c_cflag &= ~CSTOPB;
  ds->tty.c_cflag &= ~CSIZE;
  ds->tty.c_cflag |= CS8;
  ds->tty.c_cflag &= ~CRTSCTS;
  ds->tty.c_cflag |= CREAD | CLOCAL;
  ds->tty.c_lflag &= ~ICANON;
  ds->tty.c_lflag &= ~ECHO;
  ds->tty.c_lflag &= ~ECHOE;
  ds->tty.c_lflag &= ~ECHONL;
  ds->tty.c_lflag &= ~ISIG;
  ds->tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  ds->tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
  ds->tty.c_oflag &= ~OPOST;
  ds->tty.c_oflag &= ~ONLCR;
  ds->tty.c_cc[VTIME] = 10;
  ds->tty.c_cc[VMIN] = 0;

  cfsetispeed(&ds->tty, B115200);
  cfsetospeed(&ds->tty, B115200);

  //  cfmakeraw(&ds->tty);

  if(tcsetattr(ds->uart_h, TCSANOW, &ds->tty)) {
    spdlog::error("Something went wrong when SETTING port attributes...");
    return 1;
  }

  spdlog::info("UART initialized!");

  char rx_msg[64];
  ssize_t rx_bytes = read(ds->uart_h, rx_msg, 64);
  
  while(1) {
    memset(rx_msg, 0, 64);
    rx_bytes = read(ds->uart_h, rx_msg, 64);

    if(rx_bytes > 0) {
      std::string payload(rx_msg);
      ds->universal_map["uart/uartSen"] = payload; 

      // spdlog::debug("{} {}", payload, rx_msg);

      if (!ds->active_registers.count("uartSen")) {
        create_task_detached(ds, "uartSen");
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}
