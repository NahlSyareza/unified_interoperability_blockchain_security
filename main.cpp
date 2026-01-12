#include <iostream>
#include <http_handler.h>
#include <mqtt_handler.h>
#include <hitotsu.hpp>

using namespace std;

int main()
{
  cout << "Walking by the wall" << endl;

  Fucker fucker;

  fucker.superbialis = 0;

  auto http_thread = async(launch::async, http_handler, &fucker);
  auto mqtt_thread = async(launch::async, mqtt_handler, &fucker);

  http_thread.wait();
  mqtt_thread.wait();

  // http_handler();
  // mqtt_handler();

  return 0;
}
