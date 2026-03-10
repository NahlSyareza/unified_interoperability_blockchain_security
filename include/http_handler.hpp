#pragma once

#include <ble_handler.hpp>
#include <data_structure.hpp>
#include <de_ruyter.hpp>
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using string = std::string;
using json = nlohmann::json;
using Server = httplib::Server;
using Request = httplib::Request;
using Response = httplib::Response;

int http_handler(DataStructure *dstructure) {
  Server svr;

  svr.Get("/dummy", [](const Request &req, Response &res) { res.set_content("Hello World!", "text/plain"); });

  svr.Get(R"(/(.*))", [dstructure](const Request &req, Response &res) {
    string path = req.matches[0];
    path.erase(0, 1);

    json ret;

    if (dstructure->http_map.find(path) == dstructure->http_map.end()) {
      ret["state"] = false;
      ret["msg"] = "Key doesn't exist in record.";
      ret["payload"] = "";
      ret["code"] = 400;

      res.set_content(ret.dump(), "application/json");

      return;
    }

    string payload = dstructure->http_map[path]->payload;

    ret["state"] = true;
    ret["msg"] = "Successfully retrieved value.";
    ret["payload"] = payload;
    ret["code"] = 200;

    res.set_content(ret.dump(), "application/json");
  });

  svr.Post(R"(/(.*))", [dstructure](const Request &req, Response &res) {
    json ret;
    json json_body;

    string body = req.body;
    string path = req.matches[0];
    path.erase(0, 1);

    bool is_json = false;

    try {
      json_body = json::parse(body);
      is_json = true;
      spdlog::info("Retrieved body IS JSON");
      spdlog::info("HTTP: POST body {}", json_body.dump());
    } catch (json::parse_error &e) {
      spdlog::warn("Retrieved body is RAW");
      spdlog::info("HTTP: POST body {}", body);
      is_json = false;
    }

    de_ruyter(dstructure, path, body);

    res.status = 201;

    if (is_json)
      res.set_content(body, "application/json");
    else
      res.set_content(body, "text/plain");
  });

  svr.listen("0.0.0.0", 18080);

  return 0;
}