#include <data_structure.hpp>
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

int de_ruyter(DataStructure *dstructure, string source, string payload) {
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
    dstructure->insert_map_key(&dstructure->mqtt_map, selected_contents->destination, payload);
  } else if (selected_contents->protocol == "http") {
    dstructure->insert_map_key(&dstructure->http_map, selected_contents->destination, payload);
  }

  return 0;
}