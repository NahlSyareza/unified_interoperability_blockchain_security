#include <data_queues.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Contents {
  string protocol;
  string destination;
};

int de_ruyter(Queues *ques, string source, string payload) {
  ifstream f("./include/connections.json");
  map<string, Contents *> connections_map;

  json connections_file = json::parse(f);

  for (const auto &item : connections_file) {
    string source = item["source"];
    string protocol = item["protocol"];
    string destination = item["destination"];

    connections_map[source] = (Contents *)calloc(1, sizeof(Contents));
    new (&connections_map[source]->protocol) string();
    new (&connections_map[source]->destination) string();

    connections_map[source]->protocol = protocol;
    connections_map[source]->destination = destination;
  }

  Contents *selected_contents = connections_map[source];

  if (selected_contents->protocol == "mqtt") {
    ques->create_node(payload, selected_contents->destination, ques->mqttlist);
  }

  return 0;
}