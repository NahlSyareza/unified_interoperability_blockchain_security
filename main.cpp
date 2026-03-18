#include <ble_handler.hpp>
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

  auto mqtt_thread = async(launch::async, mqtt_handler, &dstructure);
  auto http_thread = async(launch::async, http_handler, &dstructure);
  auto ble_thread = async(launch::async, ble_handler, &dstructure);

  // http_thread.wait();
  // mqtt_thread.wait();
  // ble_thread.wait();

  // http_thread.get();
  // mqtt_thread.get();
  // ble_thread.get();

  return 0;
}
