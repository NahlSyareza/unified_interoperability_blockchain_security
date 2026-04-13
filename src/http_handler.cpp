#include "http_handler.hpp"

int http_handler(DataStructure *ds) {
  httplib::Server svr;

  svr.Get("/dummy", [](const httplib::Request &req [[maybe_unused]], httplib::Response &res) { res.set_content("Hello World!", "text/plain"); });

  svr.Post(R"(/handshake/(.*))", [ds](const httplib::Request &req [[maybe_unused]], httplib::Response &res) {
    std::string path = req.matches[1];
    nlohmann::json ret;

    // res.set_content(path, "text/plain");

    bool task_created = create_task_detached(ds, path, path, 3000);

    ret["state"] = task_created;
    if (task_created) {
      ret["msg"] = "Task successfully created";
      res.status = 200;
    } else {
      ret["msg"] = "Task with this name has already been created!";
      res.status = 400;
    }

    res.set_content(ret.dump(), "application/json");
  });

  svr.Get(R"(/(.*))", [ds](const httplib::Request &req, httplib::Response &res) {
    std::string path = req.matches[1];
    // path.erase(0, 1);

    nlohmann::json ret;

    // if (ds->http_map.find(path) == ds->http_map.end()) {
    if (!ds->http_map.count(path)) {
      ret["state"] = false;
      ret["msg"] = "Key doesn't exist in record.";
      ret["payload"] = "";

      res.status = 400;

      res.set_content(ret.dump(), "application/json");

      return;
    }

    bool is_json = false;
    std::string payload = ds->http_map.at(path);

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

  svr.Post(R"(/(.*))", [ds](const httplib::Request &req, httplib::Response &res) {
    nlohmann::json ret;

    nlohmann::json json_body;
    std::string body = req.body;
    std::string path = req.matches[1];
    // path.erase(0, 1);

    bool is_json = false;

    try {
      json_body = nlohmann::json::parse(body);
      is_json = true;
      spdlog::info("HTTP: POST body {}", json_body.dump());
    } catch (nlohmann::json::parse_error &e [[maybe_unused]]) {
      spdlog::info("HTTP: POST body {}", body);
      is_json = false;
    }

    // de_ruyter(ds, path, body);
    ds->http_map[path] = body;

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