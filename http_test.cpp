#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

using Server = httplib::Server;
using Request = httplib::Request;
using Response = httplib::Response;

using string = std::string;

int main() {
  // HTTP
  httplib::Server svr;

  // HTTPS
  // httplib::SSLServer svr;

  svr.Get("/hi", [](const httplib::Request &req, httplib::Response &res) { res.set_content("Hello World!", "text/plain"); });

  svr.Get(R"(/(.*))", [](const Request &req, Response &res) {
    string path = req.matches[0];
    path.erase(0, 1);

    res.set_content(path, "text/plain");
  });

  svr.listen("0.0.0.0", 18080);

  return 0;
}
