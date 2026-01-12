#include <iostream>
#include <include/data_queues.hpp>

int main()
{
  Queues que;
  que.wifilist = (Queues::LinkedList *)calloc(1, sizeof(Queues::LinkedList));

  // Queues::Node *wnode1 = que.create_node("Sayonara to", que.wifilist);
  // Queues::Node *wnode2 = que.create_node("Itta kimi no", que.wifilist);
  // Queues::Node *wnode3 = que.create_node("Kimochi ga wakaranai kedo", que.wifilist);

  que.create_node("Sayonara to", que.wifilist);
  que.create_node("Itta kimi no", que.wifilist);
  que.create_node("Kimochi ga wakaranai kedo", que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);

  printf("--------------------------------\n");

  que.pop(que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);

  printf("--------------------------------\n");

  que.pop(que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);

  printf("--------------------------------\n");

  que.pop(que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);

  printf("--------------------------------\n");

  que.pop(que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);

  printf("--------------------------------\n");

  que.pop(que.wifilist);

  que.print_linked_list(que.wifilist);

  printf("%d\n", que.wifilist->size);
}