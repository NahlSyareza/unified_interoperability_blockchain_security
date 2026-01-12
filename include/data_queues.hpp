#pragma once

#include <iostream>
#include <map>
#include <sstream>

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

  map<string, LinkedList *> httpmap;

  LinkedList *mqttlist;
  LinkedList *bltlist;

  void create_key(map<string, LinkedList *> *mp, string key, string payload) {
    if (mp->find(key) == mp->end()) {
      (*mp)[key] = (LinkedList *)calloc(1, sizeof(LinkedList));
      // cout << (*mp)[key] << endl;
    }

    create_node(payload, "", (*mp)[key]);
  }

  void create_node(string payload, string route, LinkedList *l) {
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

  LinkedList *find_list(map<string, LinkedList *> *mp, string key) {
    if (mp->find(key) != mp->end()) {
      return (*mp)[key];
    } else {
      return NULL;
    }
  }

  void print_list(LinkedList *l) {
    if (l == NULL) {
      cout << "Linked List is empty!" << endl;
      return;
    }

    Node *current = l->first;

    while (current != NULL) {
      cout << current->payload << endl;
      current = current->next;
    }
  }

  string string_list(LinkedList *l) {
    if (l == NULL) {
      cout << "Linked List is empty!" << endl;
      return "";
    }

    Node *current = l->first;
    ostringstream ss;

    while (current != NULL) {
      ss << current->payload << endl;
      current = current->next;
    }

    return ss.str();
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