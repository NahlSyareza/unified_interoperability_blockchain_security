#include <iostream>
#include <include/data_queues.hpp>

int main()
{
  Queues::WiFiList *wlist = (Queues::WiFiList *)calloc(1, sizeof(Queues::WiFiList));

  Queues::WiFiNode *wnode1 = Queues::createNode("Sayonara to", &wlist);
  Queues::WiFiNode *wnode2 = Queues::createNode("Itta kimi no", &wlist);
  Queues::WiFiNode *wnode3 = Queues::createNode("Kimochi ga wakaranai kedo", &wlist);

  Queues::printWiFiList(wlist);
}