#pragma once

#include <iostream>
#include <map>

using namespace std;

class Queues {
public:
  struct Node {
    string payload;
    string route;
    Node *next;
  };

  struct LinkedList {
    Node *first;
    Node *last;
    int size;
  };

  LinkedList *wifilist;
  LinkedList *mqttlist;

  LinkedList *bltlist;

  void create_node(string payload, LinkedList *l) {
    Node *n = (Node *)calloc(1, sizeof(Node));
    new (&n->payload) string();

    n->payload = payload;
    n->next = NULL;

    if (l->size == 0) {
      l->first = n;
      l->last = n;
    } else {
      l->last->next = n;
      l->last = n;
    }

    l->size++;

    // return n;
  }

  void print_linked_list(LinkedList *l) {
    Node *current = l->first;

    while (current != NULL) {
      cout << current->payload << endl;
      current = current->next;
    }
  }

  void pop(LinkedList *l) {
    if (l->first == NULL) {
      l->last = NULL;
      printf("Nothing in the queue!\n");
      return;
    }

    Node *sub = l->first->next;
    l->first->payload.~string();
    free(l->first);
    l->first = sub;
    l->size--;
  }

  Node *pop_tropica(LinkedList *l) {
    if (l->first == NULL) {
      l->last = NULL;
      printf("Nothing in the queue!\n");
      return NULL;
    }

    Node *ret = l->first;
    Node *sub = l->first->next;
    l->first->payload.~string();
    free(l->first);
    l->first = sub;
    l->size--;

    return ret;
  }

  void pop_print(LinkedList *l) {
    cout << l->first->payload << endl;

    pop(l);
  }
};