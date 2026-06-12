#pragma once

#include <iostream>

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
