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
    json payload;

    string body = req.body;

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

  svr.listen("0.0.0.0", 18080);

  return 0;
}