#include "crow.h"
#include <chrono>
#include <data_structure.hpp>
#include <de_ruyter.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace std;
using json = nlohmann::json;

int http_handler(DataStructure *dstructure) {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::GET)([dstructure](const crow::request &req, string path) {
    crow::json::wvalue ret;

    // DataStructure::Data *data = dstructure->http_map[path];

    if (dstructure->http_map.find(path) == dstructure->http_map.end()) {
      ret["state"] = false;
      ret["msg"] = "Key doesn't exist in record.";
      ret["payload"] = "";
      ret["code"] = 400;

      return ret;
    }

    string payload = dstructure->http_map[path]->payload;

    ret["state"] = true;
    ret["msg"] = "Successfully retrieved value.";
    ret["payload"] = payload;
    ret["code"] = 200;

    return ret;
  });

  CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::POST)([dstructure](const crow::request &req, string path) {
    crow::json::wvalue ret;
    string body = req.body;

    json payload;

    try {
      payload = json::parse(body);
    } catch (json::parse_error &e) {
      ret["state"] = false;
      ret["msg"] = "Value to be created is not a valid JSON format.";
      ret["payload"] = "";
      ret["code"] = 400;

      return ret;
    }

    de_ruyter(dstructure, path, payload.dump());

    ret["state"] = true;
    ret["msg"] = "Successfully created new value.";
    ret["payload"] = payload.dump();
    ret["code"] = 200;

    return ret;
  });

  app.port(18080).bindaddr("0.0.0.0").multithreaded().run();

  return 0;
}
