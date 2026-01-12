#include <iostream>

struct Node
{
  int data;
  Node *next;
};

void print_contents(Node *n)
{
  Node *current = n;
  while (current != NULL)
  {
    printf("%d\n", current->data);
    current = current->next;
  }
}

void createNode(Node **n, int data)
{
  *n = (Node *)calloc(1, sizeof(Node));
  (*n)->data = data;
}

void createNodeD(Node **n, int data)
{
  printf("nPtr val: %p\n", *n);
  *n = (Node *)malloc(sizeof(Node));
  printf("nPtr val: %p\n", *n);
}

int main()
{
  // Node *node1 = (Node *)calloc(1, sizeof(Node));
  // node1->data = 10;
  Node *node1;
  createNode(&node1, 10);

  Node *node2 = (Node *)calloc(1, sizeof(Node));
  node2->data = 20;

  Node *node3 = (Node *)calloc(1, sizeof(Node));
  node3->data = 30;

  node1->next = node2;
  node2->next = node3;
  node3->next = NULL;

  // print_contents(node1);

  Node *node4;
  printf("node4Ptr val: %p\n", node4);
  createNodeD(&node4, 10);
  // node4 = (Node *)malloc(sizeof(Node));
  printf("node4Ptr val: %p\n", node4);

  return 0;
}