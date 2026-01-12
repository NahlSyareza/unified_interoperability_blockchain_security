#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Simple {
  json payload;
  int id;
};

void print_simple(Simple *s) { cout << s->payload.dump() << endl; }

int main() {
  Simple *s1 = (Simple *)malloc(sizeof(Simple));
  s1->payload = {{"hello", "broman"}, {"legio", 8}};

  print_simple(s1);

  return 0;
}