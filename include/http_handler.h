#include "crow.h"
#include <iostream>
#include <hitotsu.hpp>

using namespace std;

string find_mac_addr(string ip_addr)
{
  char buff[128];

  string cmd = "cat /proc/net/arp | grep " + ip_addr + " | awk '{print $4}'";
  FILE *pipe = popen(cmd.c_str(), "r");

  if (!pipe)
  {
    return "Error Identity";
  }

  fgets(buff, sizeof(buff), pipe);

  return string(buff);
}

int http_handler(Fucker *f)
{
  crow::SimpleApp app;

  CROW_ROUTE(app, "/<path>")([f](const crow::request &req, string path)
                             {
        (*f).superbialis++;
        printf("%d\n", (*f).superbialis);
        string client_ip = req.remote_ip_address;
        return "Shot from " + client_ip + " with your name is " + find_mac_addr(client_ip) + " and hitting tower " + path; });

  app.port(18080)
      .bindaddr("0.0.0.0")
      .multithreaded()
      .run();

  return 0;
}