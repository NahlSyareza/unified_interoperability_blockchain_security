#pragma once

#include <iostream>

using string = std::string;

struct OperationRegister {
  string type;
  string input_data;
  string output_data;
  string convert;
};

// If the segment is greater than the space count inside that string, then return the whole string dawg.
string strslc(string base, int segment_count) {
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

  std::cout << "start_index: " << start_index << " end_index: " << end_index << std::endl;

  string rtn = "";
  rtn.assign(base, start_index, end_index - start_index + 1);

  return rtn;
}