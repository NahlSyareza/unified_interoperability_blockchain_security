#include <data_queues.hpp>
#include <http_handler.hpp>
#include <iostream>
#include <mqtt_handler.hpp>

using namespace std;

int main() {
  cout << "Walking by the wall" << endl;

  Queues que;
  que.wifilist = (Queues::LinkedList *)calloc(1, sizeof(Queues::LinkedList));
  que.mqttlist = (Queues::LinkedList *)calloc(1, sizeof(Queues::LinkedList));

  // que.create_node("Sayonara to", que.wifilist);
  // que.create_node("Itta kimi no", que.wifilist);
  // que.create_node("Kimochi ga wakaranai kedo", que.wifilist);

  auto http_thread = async(launch::async, http_handler, &que);
  auto mqtt_thread = async(launch::async, mqtt_handler, &que);

  http_thread.wait();
  mqtt_thread.wait();

  return 0;
}
