#include "http_handler.hpp"
// #include "httplib.h"
#include <nlohmann/json.hpp>
// #include "task_scheduler.hpp"
#include "data_route_handler.hpp"

int http_handler(DataStructure::Instance *ds) {
  ds->svr.Get(R"(/(.*))", [ds](const httplib::Request &req, httplib::Response &res) {
    std::string path = req.matches[1];
    // path.erase(0, 1);

    nlohmann::json ret;

    // if (ds->http_map.find(path) == ds->http_map.end()) {
    if (!ds->universal_map.count("http/" + path)) {
      ret["state"] = false;
      ret["msg"] = "Key doesn't exist in record.";
      ret["payload"] = "";

      res.status = 400;

      res.set_content(ret.dump(), "application/json");

      return;
    }

    bool is_json = false;
    std::string payload = ds->universal_map.at("http/" + path);

    try {
      ret = nlohmann::json::parse(payload);
      is_json = true;
      if (!ret.is_object()) {
        is_json = false;
      }
    } catch (nlohmann::json::parse_error &e [[maybe_unused]]) {
      is_json = false;
    }

    res.status = 200;

    if (is_json) {
      ret["state"] = true;
      ret["msg"] = "Successfully retrieved value";
      res.set_content(ret.dump(), "application/json");
    } else {
      res.set_content(payload, "text/plain");
    }
  });

  ds->svr.Post(R"(/(.*))", [ds](const httplib::Request &req, httplib::Response &res) {
    nlohmann::json ret;

    nlohmann::json json_body;
    std::string body = req.body;
    std::string path = req.matches[1];

    bool is_json = false;

    try {
      json_body = nlohmann::json::parse(body);
      is_json = true;
      spdlog::debug("HTTP: POST body {}", json_body.dump());
    } catch (nlohmann::json::parse_error &e [[maybe_unused]]) {
      spdlog::debug("HTTP: POST body {}", body);
      is_json = false;
    }

    ds->universal_map["http/" + path] = body;

    data_route_handler(ds, path);

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

  spdlog::info("HTTP initialized!");

  ds->svr.listen("0.0.0.0", 18080);

  return 0;
  }
