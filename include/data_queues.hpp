#pragma once

#include <iostream>

using namespace std;

namespace Queues
{
  // public:
  struct WiFiNode
  {
    string payload;
    WiFiNode *next;
  };

  struct WiFiList
  {
    WiFiNode *first;
    WiFiNode *last;
    int size;
  };

  WiFiNode *createNode(string payload, WiFiList **l)
  {
    WiFiNode *n = (WiFiNode *)calloc(1, sizeof(WiFiNode));
    new (&n->payload) string();

    n->payload = payload;

    if ((*l)->size == 0)
    {
      n->next = NULL;
      (*l)->size++;
      (*l)->first = n;
      (*l)->last = n;
    }
    else
    {
      (*l)->last->next = n;
      (*l)->last = n;
    }

    return n;
  }

  void printWiFiList(WiFiList *l)
  {
    WiFiNode *current = l->first;

    while (current != NULL)
    {
      cout << current->payload << endl;
      current = current->next;
    }
  }
};