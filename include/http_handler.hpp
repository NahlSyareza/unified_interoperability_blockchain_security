#include "crow.h"
#include <chrono>
#include <data_queues.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace std;
using json = nlohmann::json;

void rules_of_nature(Queues *ques) {
  while (true) {
    Queues::Node *http = ques->pop_tropica(ques->wifilist);
    if (http != NULL) {
      
    }

    this_thread::sleep_for(chrono::seconds(1));
  }
}

int http_handler(Queues *ques) {
  crow::SimpleApp app;

  thread t1(rules_of_nature, ques);
  t1.detach();

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::GET)([ques](const crow::request &req, string path) {
    string client_ip = req.remote_ip_address;
    // return "Shot from " + client_ip + " with your name is " + find_mac_addr(client_ip) +
    // " and hitting tower " + path;
    return "Shot from " + client_ip + " and hitting tower " + path;
  });

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::POST)([ques](const crow::request &req, string path) {
    string body = req.body;
    json body_j = json::parse(body);
    ques->create_node(body_j.dump(), ques->mqttlist);
    cout << body_j << endl;
    return body_j.dump();
  });

  app.port(18080).bindaddr("0.0.0.0").multithreaded().run();

  return 0;
}

// string find_mac_addr(string ip_addr) {
//   char buff[128];
//   string cmd = "cat /proc/net/arp | grep " + ip_addr + " | awk '{print $4}'";
//   FILE *pipe = popen(cmd.c_str(), "r");
//   if (!pipe) {
//     return "Error Identity";
//   }
//   fgets(buff, sizeof(buff), pipe);
//   return string(buff);
// }
