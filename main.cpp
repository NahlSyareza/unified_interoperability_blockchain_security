#include <data_structure.hpp>
#include <future>
#include <http_handler.hpp>
#include <iostream>
#include <map>
#include <mqtt_handler.hpp>
#include <thread>

using namespace std;

int main() {
  cout << "Walking by the wall" << endl;

  DataStructure dstructure;

  dstructure.fill_maps();

  // for (const auto &[k, v] : dstructure.format_profiles_m) {
  //   spdlog::info("{}: {}", k, v.dump(2));
  // }

  auto mqtt_thread = async(launch::async, mqtt_handler, &dstructure);
  auto http_thread = async(launch::async, http_handler, &dstructure);

  http_thread.wait();
  mqtt_thread.wait();

  return 0;
}
