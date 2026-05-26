#pragma once

#include <iostream>

struct OperationRegister {
  std::string type;
  std::string input_data;
  std::string output_data;
  std::string convert;
  bool logic_comparison;
};

std::string strget(std::string base, int segment_count);
void print_op_reg(OperationRegister *op_reg);
bool compare(std::string sign, int a, int b);
