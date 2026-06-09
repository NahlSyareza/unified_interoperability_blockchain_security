#include "data_route_handler.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include "mosquitto.h"
#include "mqtt_protocol.h"

void i2c_processor(DataStructure::Instance *ds, std::string payload) {
  write(ds->i2c_h, payload.c_str(), payload.size());
}

void uart_processor(DataStructure::Instance *ds, std::string payload) {
  write(ds->uart_h, payload.c_str(), payload.size());
}

void spi_processor(DataStructure::Instance *ds [[maybe_unused]], std::string payload [[maybe_unused]]) {
  // uint8_t rx_buffer[16];
  // uint8_t *rx_buffer;

  // ds->spi_h.xfer((uint8_t*) payload.c_str(), payload.size(), rx_buffer, 1);
}

void rf24_processor(DataStructure::Instance *ds, std::string identifier [[maybe_unused]], std::string payload) {
  ds->radio_mode = true;

  ds->radio.stopListening((uint8_t*)identifier.c_str());
  bool report = ds->radio.write(payload.c_str(), 64);
  if(report) {
  }

  ds->radio_mode = false;
}

void ble_processor_helper(DataStructure::Instance *ds, std::string identifier, std::string payload) {
  for (auto &p : ds->ble_peripherals) {
    if (p->name() == identifier) {
      nlohmann::json ble_uuids = ds->ble_addresses[identifier];
      std::string service_uuid = ble_uuids["service"];
      std::string characteristic_uuid = ble_uuids["characteristic"];

      try {
        auto characteristic = p->get_characteristic(service_uuid, characteristic_uuid);
        SimpleBluez::ByteArray byte_array(payload.begin(), payload.end());

        characteristic->write_request(byte_array);
      } catch (std::exception &e) {
        spdlog::error("{}", e.what());
      }
    }
  }
}

void ble_processor(DataStructure::Instance *ds, std::string identifier, std::string payload) {
  std::thread ble_write_thread(ble_processor_helper, ds, identifier, payload);
  ble_write_thread.detach();
  // for (auto &p : ds->ble_peripherals) {
  //   if (p->name() == identifier) {
  //     nlohmann::json ble_uuids = ds->ble_addresses[identifier];
  //     std::string service_uuid = ble_uuids["service"];
  //     std::string characteristic_uuid = ble_uuids["characteristic"];
  //     //      spdlog::info("BLE {} Service: {} Characteristic {} ", identifier, service_uuid, characteristic_uuid);

  //     try {
  //       auto characteristic = p->get_characteristic(service_uuid, characteristic_uuid);
  //       SimpleBluez::ByteArray byte_array(payload.begin(), payload.end());

  //       characteristic->write_request(byte_array);
  //     } catch (std::exception &e) {
  //       spdlog::error("{}", e.what());
  //     }
  //   }

  //   // p.write_request(ds->uuid_pair[identifier].first, ds->uuid_pair[identifier].second, payload);
  // }
}

int resolve_address(coap_str_const_t *host, uint16_t port, coap_address_t *dst,
    int scheme_hint_bits) {
  int ret = 0;
  coap_addr_info_t *addr_info;

  addr_info = coap_resolve_address_info(host, port, port,  port, port,
      AF_UNSPEC, scheme_hint_bits,
      COAP_RESOLVE_TYPE_REMOTE);
  if (addr_info) {
    ret = 1;
    *dst = addr_info->addr;
  }

  coap_free_address_info(addr_info);
  return ret;
}

void coap_processor(DataStructure::Instance *ds, std::string query, std::string payload, std::string ip) {
  coap_uri_t uri;
  coap_address_t dst;
  coap_optlist_t *optlist = nullptr;
  coap_pdu_t *pdu = nullptr;
  unsigned char scratch[100];
  int res;
  int len;

  std::string final_ip = "coap://" + ip + "/" + query;

  len = coap_split_uri((const unsigned char*)final_ip.c_str(), final_ip.size(), &uri);
  if(len) {
    spdlog::error("Failed to parse uri!");
    return;
  }

  len = resolve_address(&uri.host, uri.port, &dst, 1 << uri.scheme);

  if(len <= 0) {
    spdlog::error("Faile to resolve address");
  }

  int is_mcast = coap_is_mcast(&dst);

  if(ds->coap_sess == nullptr) {
    ds->coap_sess = coap_new_client_session(ds->coap_ctx, NULL, &dst, COAP_PROTO_UDP);
  }

  pdu = coap_pdu_init(is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST, coap_new_message_id(ds->coap_sess), coap_session_max_pdu_size(ds->coap_sess));

  if(!pdu) {
    spdlog::error("Cannot create pdu!");
    return;
  }

  if(!optlist) {
    len = coap_uri_into_options(&uri, &dst, &optlist, 1, scratch, sizeof(scratch));

    if(len) {
      spdlog::error("Failed to create options!");

      return;
    }
  }

  if(optlist) {
    res = coap_add_optlist_pdu(pdu, &optlist);
    if(res != 1) {
      spdlog::error("Failed to add options list!");
      return;
    }
  }

  coap_add_data(pdu, payload.size(), (const uint8_t*)payload.c_str());

  if(coap_send(ds->coap_sess, pdu) == COAP_INVALID_MID) {
    spdlog::error("Cannot send CoAP PDU!");
    return;
  }

  coap_delete_optlist(optlist);
}

void mqtt_processor(DataStructure::Instance *ds, std::string topic, std::string payload) {
  int rc;

  mosquitto_property *proplist = NULL;
  rc = mosquitto_property_add_string_pair(&proplist, MQTT_PROP_USER_PROPERTY, "origin", "external");
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Something's wrong I can feel it");
  }

  rc = mosquitto_publish(ds->mosq, nullptr, topic.c_str(), (int)payload.length(), payload.c_str(), 2, false);
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  }
}

void http_processor(DataStructure::Instance *ds, std::string path, std::string payload) { ds->universal_map["http/" + path] = payload; }

void get_instr(std::string op, std::string payload, OperationRegister *reg) {
  size_t ctrl;
  std::string instr, act;

  // Perhaps find a way to detect a non-space first character
  if (op.at(0) == ';') {
    return;
  }

  for (ctrl = 0; ctrl < op.length(); ctrl++) {
    if (op.at(ctrl) == ' ' || op.at(ctrl) == '\n') {
      break;
    }
  }

  instr.assign(op, 0, ctrl);

  if (ctrl + 1 < op.length()) {
    act.assign(op, ctrl + 1, op.length() - ctrl + 1);
  } else {
    // std::cout << "Illegal operation attempted. Maybe caused by incomplete op." << std::endl;
  }

  // std::cout << "Instruction: " << instr << std::endl;
  // std::cout << "Act: " << act << std::endl;

  process_instr(instr, act, payload, reg);
}

void process_instr(std::string instr, std::string act, std::string payload, OperationRegister *reg) {
  // spdlog::debug("Running instruction: {}", instr);

  if(instr == "DELIM") {
    reg->delimiter = act.at(0);
    fprintf(stdout, "%c\n", reg->delimiter);
  } else if (instr == "GET") {
    int detects = std::stoi(act);
    reg->input_data = strget(payload, detects, reg->delimiter);
  } else if (instr == "GET_FROM") {
    nlohmann::json json_obj;

    try {
      json_obj = nlohmann::json::parse(payload);
      if (!json_obj.is_object()) {
        // This shouldn't happen to be honest
        throw nlohmann::json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const nlohmann::json::parse_error &e) {
      spdlog::error("(GET_FROM) {}", e.what());
    }

    reg->input_data = json_obj[act].dump();
  } else if (instr == "TYPE") {
    reg->type = act;
  } else if (instr == "ASGN_TO") {
    nlohmann::json json_obj;

    try {
      json_obj = nlohmann::json::parse(reg->output_data);
      if (!json_obj.is_object()) {
        throw nlohmann::json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const nlohmann::json::parse_error &e [[maybe_unused]]) {
      // spdlog::error("(ASGN_TO) {}", e.what());
      json_obj = nlohmann::json::parse("{}");
    }

    try {
      if (reg->convert == "int") {
        json_obj[act] = std::stoi(reg->input_data);
      } else if (reg->convert == "double") {
        json_obj[act] = std::stod(reg->input_data);
      } else {
        json_obj[act] = reg->input_data;
      }
    } catch (const std::invalid_argument &e) {
      // std::cout << e.what() << std::endl;
      spdlog::error("(ASGN_TO) {}", e.what());
      json_obj[act] = reg->input_data;
    }

    reg->convert = "";
    reg->output_data = json_obj.dump();
  } else if (instr == "IF") {
    std::string compared_value = strget(act, 1, ' ');
    // reg->logic_comparison = compare(act, std::stoi(reg->input_data), std::stoi(compared_value));
    reg->logic_comparison = compare(act, reg->type, reg->input_data, compared_value);
  } else if (instr == "ELSEIF") {
    if (!reg->logic_comparison) {
      std::string compared_value = strget(act, 1, ' ');
      // reg->logic_comparison = compare(act, std::stoi(reg->input_data), std::stoi(compared_value));
      reg->logic_comparison = compare(act, reg->type, reg->input_data, compared_value);
    }
  } else if (instr == "CONV_TO") {
    reg->convert = act;
  } else if (instr == "THEN") {
    if (reg->logic_comparison)
      reg->input_data = act;
  } else if (instr == "OTHERWISE") {
    if (!reg->logic_comparison)
      reg->input_data = act;
  } else if(instr == "OUT") {
    reg->output_data = reg->input_data;
  } else if(instr == "APPEND_OUT") {
    reg->output_data.append(" " + reg->input_data);
  } else {
    spdlog::error("(Process Instr) Unrecognized instruction {}", instr);
  }
}

void extract_config(std::string path, nlohmann::json *json_ptr) {
  std::ifstream file(path);

  if(file.fail()) {
    spdlog::error("(Extract Config) Error cannot open file path {}", path);
  }

  *json_ptr = nlohmann::json::parse(file);
  file.close();
}

bool create_interop_data(DataStructure::Instance *ds, std::string source, nlohmann::json *json_ptr) {
  nlohmann::json connection_registers, instance_registers, network_profiles, format_profiles;

  extract_config("./config/connection_registers.json", &connection_registers);
  extract_config("./config/instance_registers.json", &instance_registers);
  extract_config("./config/network_profiles.json", &network_profiles);
  extract_config("./config/format_profiles.json", &format_profiles);

  if(!connection_registers.count(source)) {
    spdlog::error("(Create Interop Data) Unknown source: {}!", source);

    return false;
  }

  std::string destination = connection_registers[source]["destination"];

  // (*json_ptr)["interval"] = connection_registers[source]["interval"];

  if (connection_registers[source].count("rules")) {
    (*json_ptr)["rules"] = connection_registers[source]["rules"];
  } else {
    (*json_ptr)["rules"] = "";
  }

  nlohmann::json data;
  data["name"] = source;

  if (!instance_registers.count(source)) {
    spdlog::error("Source instance {} is not recognized!", source);
    json_ptr->clear();
    return false;
  } else if (!instance_registers.count(destination)) {
    spdlog::error("Destination instance {} is not recognized!", destination);
    json_ptr->clear();
    return false;
  }

  std::string network = instance_registers[source]["network"];
  if (!network_profiles.count(network)) {
    spdlog::error("Source network profile {} is not found!", network);
    json_ptr->clear();
    return false;
  }
  data["network"] = network_profiles[network];

  std::string format = instance_registers[source]["format"];
  if (!format_profiles.count(format)) {
    spdlog::error("Source format profile {} is nout found!", format);
    json_ptr->clear();
    return false;
  }
  data["format"] = format_profiles[format];

  (*json_ptr)["src"] = data;

  data["name"] = destination;

  if(destination == "coapAct") {
    data["ip"] = instance_registers[destination]["ip"];
  }

  network = instance_registers[destination]["network"];
  if (!network_profiles.count(network)) {
    spdlog::error("Destination network profile {} is not found!", network);
    json_ptr->clear();
    return false;
  }

  data["network"] = network_profiles[network];

  format = instance_registers[destination]["format"];
  if (!format_profiles.count(format)) {
    spdlog::error("Destination format profile {} is not found!", format);
    json_ptr->clear();
    return false;
  }
  data["format"] = format_profiles[format];

  (*json_ptr)["dst"] = data;

  ds->interop_map[source] = *json_ptr;

  return true;
}


void data_route_handler(DataStructure::Instance *ds, std::string source) {
  nlohmann::json interop_data;

  if(!ds->interop_map.count(source)) {
    create_interop_data(ds, source, &interop_data);
  } else {
    interop_data = ds->interop_map[source];
  }
  // create_interop_data(source, &interop_data);

  // spdlog::debug("{}", interop_data.dump(2));

  nlohmann::json src = interop_data["src"];
  nlohmann::json dst = interop_data["dst"];

  std::string src_conn = src["network"]["connection"];
  std::string src_name = src["name"];
  std::string dest_conn = dst["network"]["connection"];
  std::string dst_name = dst["name"];
  std::string rules = interop_data["rules"];
  std::ifstream rule_file("./rules/" + rules);
  std::stringstream ss;
  std::string payload;
  std::string line;

  std::string map_location = "";
  map_location.append(src_conn).append("/").append(src_name);

  payload = ds->universal_map[src_conn + "/" + src_name];

  OperationRegister op_reg;

  if (rule_file && rule_file.peek() != EOF) {
    ss << rule_file.rdbuf();
    rule_file.close();

    while (std::getline(ss, line)) {
      get_instr(line, payload, &op_reg);
    }

  }

  std::string final_payload = !op_reg.output_data.empty() ? op_reg.output_data : payload;

  if (dest_conn == "coap") {
    std::string ip = dst["ip"];
    coap_processor(ds, dst_name, final_payload, ip);
  } else if (dest_conn == "mqtt") {
    mqtt_processor(ds, dst_name, final_payload);
  } else if (dest_conn == "ble") {
    ble_processor(ds, dst_name, final_payload);
  } else if (dest_conn == "rf24") {
    rf24_processor(ds, dst_name, final_payload);
  } else if (dest_conn == "uart") {
    uart_processor(ds, final_payload);   
  } else if (dest_conn == "i2c") {
    i2c_processor(ds, final_payload);
  } else if (dest_conn == "spi") {
    spi_processor(ds, final_payload);
  }

  if(ds->pr_time) {
    ds->end_time = std::chrono::high_resolution_clock::now();;
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(ds->end_time - ds->start_time);

    ds->save_pr_time();
  }
}
