#include <data_queues.hpp>
#include <de_ruyter.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using namespace std;

int main() {
  Queues ques;
  ques.mqttlist = (Queues::LinkedList *)calloc(1, sizeof(Queues::LinkedList));

  de_ruyter(&ques, "device-1/test", "Free will");

  return 0;
}