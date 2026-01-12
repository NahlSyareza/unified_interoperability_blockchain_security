#include <data_queues.hpp>
#include <iostream>

int main() {
  Queues ques;
  ques.create_key(&ques.httpmap, "strongman", "superbus");
  cout << ques.httpmap["strongman"] << endl;

  ques.print_list(ques.httpmap["strongman"]);

  return 0;
}