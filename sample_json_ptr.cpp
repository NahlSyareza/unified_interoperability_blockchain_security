#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using string = std::string;
using ifstream = std::ifstream;
using json = nlohmann::json;

void json_ptr_test(json *pass_the_dutchie) {
  json deref = (*pass_the_dutchie);

  cout << deref["cqc_capabilities"] << endl;
}

int main() {
  ifstream sample_json("./sample.json");
  json parse_sample_json = json::parse(sample_json);

  cout << parse_sample_json["cqc_capabilities"] << endl;

  json_ptr_test(&parse_sample_json);

  return 0;
}
