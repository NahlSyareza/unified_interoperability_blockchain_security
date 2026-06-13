#pragma once

#include <iostream>
#include <vector>

struct OperationRegister {
  char delimiter;
  std::string type;
  std::string input_data;
  std::string output_data;
  std::string convert;
  bool logic_comparison;
};

std::string strget(std::string base, int segment_count, char delimiter);
void print_op_reg(OperationRegister *op_reg);
bool compare(std::string sign, std::string type, std::string a, std::string b);
std::string unit_conversion(std::string command, std::string a);
bool verify_sign(std::string payload, std::string sign, std::vector<unsigned char> public_key);
std::vector<unsigned char> encode_the_key(std::string the_key);
