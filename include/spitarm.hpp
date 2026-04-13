#pragma once

#include "spdlog/spdlog.h"
#include <iostream>

using string = std::string;

struct OperationRegister {
  string type;
  string input_data;
  string output_data;
  string convert;
  bool logic_comparison;
};

string strget(string base, int segment_count);
void print_op_reg(OperationRegister *op_reg);
bool compare(string sign, int a, int b);
