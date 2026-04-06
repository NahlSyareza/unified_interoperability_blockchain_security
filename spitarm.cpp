#include "spitarm.hpp"
#include "spdlog/spdlog.h"

void print_op_reg(OperationRegister *op_reg) {
  spdlog::debug("\ntype: \"{}\"\ninput_data: \"{}\"\noutput_data: \"{}\"\nconvert: \"{}\"\nlogic_comparison: \"{}\"\n", op_reg->type, op_reg->input_data, op_reg->output_data, op_reg->convert,
                op_reg->logic_comparison);
}

// If the segment is greater than the space count inside that string, then return the whole string dawg.
string strget(string base, int segment_count) {
  int internal_segment_counter, start_index, end_index;

  internal_segment_counter = 0;
  start_index = 0;
  end_index = -1;

  for (int i = 0; i < base.length(); i++) {
    if (base.at(i) == ' ') {
      if (segment_count == internal_segment_counter) {
        end_index = i - 1;
        break;
      } else if (internal_segment_counter == segment_count - 1) {
        start_index = i + 1;
      }

      internal_segment_counter++;
    }

    if (i == base.length() - 1) {
      end_index = i;
      break;
    }
  }

  spdlog::debug("start_index: {} end_index: {}", start_index, end_index);
  // std::cout << "start_index: " << start_index << " end_index: " << end_index << std::endl;

  string rtn = "";
  rtn.assign(base, start_index, end_index - start_index + 1);

  return rtn;
}

bool compare(string sign, int a, int b) {
  // b is the one defined inside the instruction text

  if (sign.at(1) == '=') {
    if (sign.at(0) == '>') {
      return a >= b;
    } else if (sign.at(0) == '<') {
      return a <= b;
    }
  } else if (sign.at(0) == '<') {
    return a < b;
  } else if (sign.at(0) == '>') {
    return a > b;
  }

  spdlog::error("(Spitarm) Sign not recognized!");
  return false;
}