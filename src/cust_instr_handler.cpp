#include "cust_instr_handler.hpp"
#include "spdlog/spdlog.h"

void print_op_reg(OperationRegister *op_reg) {
  spdlog::debug("\ntype: \"{}\"\ninput_data: \"{}\"\noutput_data: \"{}\"\nconvert: \"{}\"\nlogic_comparison: \"{}\"\n", op_reg->type, op_reg->input_data, op_reg->output_data, op_reg->convert,
      op_reg->logic_comparison);
}

// If the segment is greater than the space count inside that std::string, then return the whole std::string dawg.
std::string strget(std::string base, int segment_count) {
  int internal_segment_counter, start_index, end_index;

  internal_segment_counter = 0;
  start_index = 0;
  end_index = -1;

  for (size_t i = 0; i < base.length(); i++) {
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

  // spdlog::debug("start_index: {} end_index: {}", start_index, end_index);
  // std::cout << "start_index: " << start_index << " end_index: " << end_index << std::endl;

  std::string rtn = "";
  rtn.assign(base, start_index, end_index - start_index + 1);

  return rtn;
}

bool compare(std::string sign, std::string type, std::string a, std::string b) {
  // b is the one defined inside the instruction text

  if(type == "char") {
    return a == b;
  } else if (type == "int")  {
    int ia = std::stoi(a);
    int ib = std::stoi(b); 

    if (sign.at(1) == '=') {
      if (sign.at(0) == '>') {
        return ia >= ib;
      } else if (sign.at(0) == '<') {
        return ia <= ib;
      }
    } else if (sign.at(0) == '<') {
      return ia < ib;
    } else if (sign.at(0) == '>') {
      return ia > ib;
    }
  } else if (type == "double") {
    double da = std::stod(a);
    double db = std::stod(b); 

    if (sign.at(1) == '=') {
      if (sign.at(0) == '>') {
        return da >= db;
      } else if (sign.at(0) == '<') {
        return da <= db;
      }
    } else if (sign.at(0) == '<') {
      return da < db;
    } else if (sign.at(0) == '>') {
      return da > db;
    }
  }

  spdlog::error("(Cust Instr Handler) Sign not recognized!");
  return false;
}
