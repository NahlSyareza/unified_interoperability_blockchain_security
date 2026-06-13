#include "blockchain_handler.hpp"

std::string decode_bytes(std::string input) {
  std::string decoded;
  decoded.reserve(input.length() / 2);
  for (size_t i = 0; i < input.length(); i += 2) {
    std::string byte_string = input.substr(i, 2);
    char byte = static_cast<char>(std::stoul(byte_string, nullptr, 16));
    decoded += byte;
  }
  return decoded;
}

void fill_device_keys(std::string data, nlohmann::json* json_ptr) {
  std::string buffer;
  std::string key;
  bool finding_key = true;

  for (char c : data) {
    if (std::isalnum(static_cast<unsigned char>(c))) {
      buffer += c;
    } else {
      if (!buffer.empty()) {
        if (finding_key) {
          key = buffer;
          finding_key = false; 
        } else {
          (*json_ptr)[key] = buffer; 
          finding_key = true;  
        }
        buffer.clear(); 
      }
    }
  }

  if (!buffer.empty() && !finding_key) {
    (*json_ptr)[key] = buffer;
  }
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
  size_t newLength = size * nmemb;
  try
  {
    s->append((char *)contents, newLength);
  }
  catch (std::bad_alloc &e)
  {
    return 0;
  }
  return newLength;
}

std::string fetch_blockchain(std::string body)
{
  CURL *curl;
  CURLcode res [[maybe_unused]];
  std::string buffer;

  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.250.1:9944");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }

  return buffer;
}

int blockchain_handler(DataStructure::Instance *ds) {
  nlohmann::json body;

  body["id"] = 1;
  body["jsonrpc"] = "2.0";
  body["method"] = "chain_getBlockHash";
  body["params"] = {1};

  std::string response = fetch_blockchain(body.dump());

  nlohmann::json json_test = nlohmann::json::parse(response);

  body["id"] = 1;
  body["jsonrpc"] = "2.0";
  body["method"] = "chain_getBlock";
  body["params"] = {json_test["result"]};

  response = fetch_blockchain(body.dump());

  json_test = nlohmann::json::parse(response);

  std::string data = json_test["result"]["block"]["extrinsics"][1];
  std::string trim_data = data.substr(222);

  std::string decoded_data = decode_bytes(trim_data);
  fill_device_keys(decoded_data, &ds->device_keys);

  std::cout << ds->device_keys.dump(2) << std::endl;

  return 0;
}
