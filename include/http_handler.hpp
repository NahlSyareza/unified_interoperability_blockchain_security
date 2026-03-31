#pragma once

#include "ble_handler.hpp"
#include "data_structure.hpp"
#include "de_ruyter.hpp"
#include "httplib/httplib.h"
#include "nlohmann/json.hpp"
#include <iostream>

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
      // ret["code"] = 400;

      res.status = 400;

      res.set_content(ret.dump(), "application/json");

      return;
    }

    // json json_payload;
    bool is_json = false;
    string payload = dstructure->http_map.at(path);

    try {
      ret = json::parse(payload);
      is_json = true;
      if (!ret.is_object()) {
        is_json = false;
      }
    } catch (json::parse_error &e) {
      is_json = false;
    }

    res.status = 200;

    if (is_json) {
      ret["state"] = true;
      ret["msg"] = "Successfully retrieved value";
      res.set_content(ret.dump(), "application/json");
    } else {
      // spdlog::warn("Payload is {}", payload);
      res.set_content(payload, "text/plain");
    }

    // ret["state"] = true;
    // ret["msg"] = "Successfully retrieved value.";
    // if (is_json)
    //   ret["payload"] = json_payload;
    // else
    //   ret["payload"] = payload;
    // ret["code"] = 200;

    // res.status = 200;

    // res.set_content(ret.dump(), "application/json");
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

    ret["state"] = true;
    ret["msg"] = "Successfully posted new data";
    if (is_json)
      ret["payload"] = json_body;
    else
      ret["payload"] = body;
    ret["code"] = 201;

    res.status = 201;

    res.set_content(ret.dump(), "application/json");
  });

  svr.listen("0.0.0.0", 18080);

  return 0;
}