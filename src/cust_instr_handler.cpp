#include "cust_instr_handler.hpp"
#include "sodium.h"
#include <spdlog/spdlog.h>

void print_op_reg(OperationRegister *op_reg) {
  spdlog::debug("\ntype: \"{}\"\ninput_data: \"{}\"\noutput_data: \"{}\"\nconvert: \"{}\"\nlogic_comparison: \"{}\"\n", op_reg->type, op_reg->input_data, op_reg->output_data, op_reg->convert,
      op_reg->logic_comparison);
}

// If the segment is greater than the space count inside that std::string, then return the whole std::string dawg.
std::string strget(std::string base, int segment_count, char delimiter) {
  int internal_segment_counter, start_index, end_index;

  internal_segment_counter = 0;
  start_index = 0;
  end_index = -1;

  for (size_t i = 0; i < base.length(); i++) {
    if (base.at(i) == delimiter) {
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
  } else if (type == "float") {
    float da = std::stof(a);
    float db = std::stof(b); 

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

std::string unit_conversion(std::string command, std::string a) {
  float data = std::stof(a);
  float result = 0;

  if (command == "tctf") {           
    result = (data * 9.0f / 5.0f) + 32.0f;
  } else if (command == "tctk") {    
    result = data + 273.15f;
  } else if (command == "tftc") {      
    result = (data - 32.0f) * 5.0f / 9.0f;
  } else if (command == "tftk") {    
    result = (data - 32.0f) * 5.0f / 9.0f + 273.15f;
  } else if (command == "tktc") {    
    result = data - 273.15f;
  } else if (command == "tktf") {    
    result = (data - 273.15f) * 9.0f / 5.0f + 32.0f;
  } else if (command == "dmtmi") {
    result = data / 1609.344f;
  } else if (command == "dmitm") {   
    result = data * 1609.344f;
  }

  return std::to_string(result);
}

std::vector<unsigned char> encode_the_key(std::string the_key) {
  std::vector<unsigned char> bytes;
  for (size_t i = 0; i < the_key.length(); i += 2) {
    std::string byteString = the_key.substr(i, 2);
    unsigned char byte = (unsigned char) strtol(byteString.c_str(), nullptr, 16);
    bytes.push_back(byte);
  }
  return bytes;
}

bool verify_sign(std::string payload [[maybe_unused]], std::string sign [[maybe_unused]], std::vector<unsigned char> public_key [[maybe_unused]]) {
    unsigned char raw_signature[crypto_sign_BYTES];

  // 1. Convert the Hex signature back into raw bytes for mathematical verification
  if (sodium_hex2bin(raw_signature, sizeof(raw_signature), 
        sign.c_str(), sign.length(), 
        nullptr, nullptr, nullptr) != 0) {
    std::cerr << "[ERROR] Invalid Hex format in signature." << std::endl;
    return false;
  }

  // 2. Run the Ed25519 verification algorithm
  // Returns 0 if the signature matches the payload and public key perfectly
  int result = crypto_sign_verify_detached(raw_signature, 
      (const unsigned char*)payload.c_str(), 
      payload.length(), 
      public_key.data());

  return (result == 0);
}
