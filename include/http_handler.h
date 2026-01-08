#include <iostream>

using namespace std;

string find_mac_addr(string ip_addr) {
  char buff[128];

  string cmd = "cat /proc/net/arp | grep " + ip_addr + " | awk '{print $4}'";
  FILE *pipe = popen(cmd.c_str(), "r");

  fgets(buff, sizeof(buff), pipe);

  return string(buff);
}
