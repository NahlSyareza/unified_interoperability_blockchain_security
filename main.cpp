#include <data_structure.hpp>
#include <http_handler.hpp>
#include <iostream>
#include <map>
#include <mqtt_handler.hpp>

using namespace std;

int main() {
  cout << "Walking by the wall" << endl;

  DataStructure dstructure;

  auto http_thread = async(launch::async, http_handler, &dstructure);
  auto mqtt_thread = async(launch::async, mqtt_handler, &dstructure);

  http_thread.wait();
  mqtt_thread.wait();

  return 0;
}
