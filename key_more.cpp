#include <iostream>
#include <map>

using namespace std;

int main() {
  map<string, int> test_map;

  test_map["keyvu"] = 10;
  test_map["machro"] = 90;

  for (const auto &[k, v] : test_map) {
    cout << k << endl;
  }

  return 0;
}