#include "crow.h"
#include <chrono>
#include <data_structure.hpp>
#include <de_ruyter.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace std;
using json = nlohmann::json;

// void rules_of_nature(Queues *ques) {
//   while (true) {
//     Queues::Node *http = ques->pop_tropica(ques->wifilist);
//     if (http != NULL) {
//     }
//     this_thread::sleep_for(chrono::seconds(1));
//   }
// }

int http_handler(DataStructure *dstructure) {
  crow::SimpleApp app;

  // thread t1(rules_of_nature, ques);
  // t1.detach();

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::GET)([dstructure](const crow::request &req, string path) {
    string payload = dstructure->http_map[path]->payload;

    crow::json::wvalue ret;
    ret["success"] = true;
    ret["payload"] = payload;
    ret["code"] = 200;

    return ret;
  });

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::POST)([dstructure](const crow::request &req, string path) {
    string body = req.body;
    json payload = json::parse(body);

    de_ruyter(dstructure, path, payload.dump());

    crow::json::wvalue ret;
    ret["success"] = true;
    ret["payload"] = payload.dump();
    ret["code"] = 200;

    return ret;
  });

  app.port(18080).bindaddr("0.0.0.0").multithreaded().run();

  return 0;
}
