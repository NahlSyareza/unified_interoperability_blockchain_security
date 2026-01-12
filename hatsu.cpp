#include <iostream>
#include <map>
#include <string>

using namespace std;

struct Node {
  string data;
  Node *next;
};

struct LinkedList {
  Node *first;
  Node *last;
  int size;
};

void create_node(LinkedList *list, string data) {
  Node *n = (Node *)calloc(1, sizeof(Node));
  new (&n->data) string();

  n->data = data;
  n->next = NULL;

  if (list->size == 0) {
    list->first = n;
    list->last = n;
  } else {
    list->last->next = n;
    list->last = n;
  }

  list->size++;
}

void print_list(LinkedList *list) {
  Node *current = list->first;

  while (current != NULL) {
    printf("%s\n", current->data.c_str());
    current = current->next;
  }
}

int main() {
  map<string, LinkedList *> httpmap;

  httpmap["strongman"] = (LinkedList *)calloc(1, sizeof(LinkedList));

  create_node(httpmap["strongman"], "Heat of the desert");
  create_node(httpmap["strongman"], "Dust settles on my face");
  create_node(httpmap["strongman"], "Without a compass");
  create_node(httpmap["strongman"], "A soldier knows no disgrace");

  print_list(httpmap["strongman"]);

  if (httpmap.find("strongman") == httpmap.end()) {
    printf("Out of the ashes, the eagle rises still!\n");
  }

  return 0;
}