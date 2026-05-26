#include "gpio_handler.hpp"

#ifdef INPUT
#undef INPUT
#endif
#ifdef OUTPUT
#undef OUTPUT
#endif

#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.hpp>

#define FS_PIN 17
#define FA_PIN 27

int gpio_handler(DataStructure::Instance* ds [[maybe_unused]]) {
  //  uint8_t FS_PIN = 17;
  //  uint8_t FA_PIN = 27;

  gpiod::line_settings fs_p_s;
  gpiod::line_settings fa_p_s;

  fs_p_s.set_direction(gpiod::line::direction::INPUT);
  fs_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  fa_p_s.set_direction(gpiod::line::direction::INPUT);
  fa_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  auto req = gpiod::chip(CHIP_NAME).prepare_request().set_consumer("legendary_boss_gpio").add_line_settings(FS_PIN, fs_p_s).add_line_settings(FA_PIN, fa_p_s).do_request();

  gpiod::line::value fs_v = req.get_value(FS_PIN);
  gpiod::line::value fa_v = req.get_value(FA_PIN);

  int counter = 0;

  while(1) {
    fs_v = req.get_value(FS_PIN);
    while(fs_v == gpiod::line::value::INACTIVE) {
      fs_v = req.get_value(FS_PIN);
    }
    std::cout << "(" << counter << ") " << "From sensor: " << ds->time_now() << std::endl;

    fa_v = req.get_value(FA_PIN);
    while(fa_v == gpiod::line::value::INACTIVE) {
      fa_v = req.get_value(FA_PIN);
    }
    std::cout << "(" << counter << ") " << "From actuator: " << ds->time_now() << std::endl;

    counter++;

//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;

}
