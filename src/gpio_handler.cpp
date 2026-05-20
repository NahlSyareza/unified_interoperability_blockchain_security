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

// const auto epoch_point = std::chrono::high_resolution_clock::now();

long time_now() {
  auto current_point = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(current_point - epoch_point);
  return dur.count();
}

int gpio_handler(DataStructure::Instance* ds [[maybe_unused]]) {
  long start, end;

  uint8_t fs_pin = 17;
  uint8_t fa_pin = 27;

  gpiod::line_settings fs_p_s;
  gpiod::line_settings fa_p_s;

  fs_p_s.set_direction(gpiod::line::direction::INPUT);
  fs_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  fa_p_s.set_direction(gpiod::line::direction::INPUT);
  fa_p_s.set_bias(gpiod::line::bias::PULL_DOWN);

  auto req = gpiod::chip(CHIP_NAME).prepare_request().set_consumer("legendary_boss_gpio").add_line_settings(fs_pin, fs_p_s).add_line_settings(fa_pin, fa_p_s).do_request();

  gpiod::line::value fs_v = req.get_value(fs_pin);
  gpiod::line::value fa_v [[maybe_unused]] = req.get_value(fa_pin);

  while(fs_v == gpiod::line::value::INACTIVE){
    fs_v = req.get_value(fs_pin);
    fa_v = req.get_value(fa_pin);
  }

  start = time_now();

  std::this_thread::sleep_for(std::chrono::milliseconds(1657));

  end = time_now();

  std::cout << "Time taken: " << end - start << std::endl;

  return 0;

}
