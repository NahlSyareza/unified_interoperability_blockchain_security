#include <iostream>
#include <cstring>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "data_structure.hpp"

int blockchain_handler(DataStructure::Instance *ds);
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s);
std::string fetch_blockchain(std::string body);
std::string decode_bytes(std::string input);
void fill_public_keys(std::string data, nlohmann::json* json_ptr);

