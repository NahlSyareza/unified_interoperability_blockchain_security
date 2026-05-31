#include "gpio_handler.hpp"

#ifdef INPUT
#undef INPUT
#endif
#ifdef OUTPUT
#undef OUTPUT
#endif

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <gpiod.hpp>

#define FS_PIN 17
#define FD_PIN 27

auto epoch_point = std::chrono::high_resolution_clock::now();

long get_ms() {
  auto current_point = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(current_point - epoch_point);
  return dur.count();
}

int gpio_handler() {
  long source_time, destination_time;

  gpiod::line_settings fs_p_s;
  gpiod::line_settings fd_p_s;

  fs_p_s.set_direction(gpiod::line::direction::INPUT);
  fs_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  fd_p_s.set_direction(gpiod::line::direction::INPUT);
  fd_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  auto req = gpiod::chip(CHIP_NAME).prepare_request().set_consumer("legendary_boss_gpio").add_line_settings(FS_PIN, fs_p_s).add_line_settings(FD_PIN, fd_p_s).do_request();

  gpiod::line::value fs_v = req.get_value(FS_PIN);
  gpiod::line::value fd_v = req.get_value(FD_PIN);

  int counter = 0;

  while(1) {
    fs_v = req.get_value(FS_PIN);
    while(fs_v == gpiod::line::value::INACTIVE) {
      fs_v = req.get_value(FS_PIN);
    }
    source_time = get_ms();
    printf("(%d) Source: %ld\n", counter, source_time);
    // std::cout<< "(" << counter << ") " << "From sensor: " << ds->time_now() << std::endl;

    fd_v = req.get_value(FD_PIN);
    while(fd_v == gpiod::line::value::INACTIVE) {
      fd_v = req.get_value(FD_PIN);
    }
    destination_time = get_ms();
    printf("(%d) Destination: %ld\n", counter, destination_time);
    // std::cout << "(" << counter << ") " << "From actuator: " << ds->time_now() << std::endl;

    counter++;
  }

  return 0;

}
