#include "de_ruyter.hpp"


void ble_processor(DataStructure *dstructure, std::string identifier, std::string payload) {
  for (auto &p : dstructure->peripherals) {
    if (p.identifier() == identifier)
      p.write_request(dstructure->uuid_pair[identifier].first, dstructure->uuid_pair[identifier].second, payload);
  }
}

void mqtt_processor(DataStructure *dstructure, std::string topic, std::string payload) {
  int rc;

  mosquitto_property *proplist = NULL;
  rc = mosquitto_property_add_string_pair(&proplist, MQTT_PROP_USER_PROPERTY, "origin", "external");
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Something's wrong I can feel it");
  }

  rc = mosquitto_publish(dstructure->mosq, nullptr, topic.c_str(), (int)payload.length(), payload.c_str(), 2, false);
  if (rc != MOSQ_ERR_SUCCESS) {
    spdlog::error("Error publishing: {}", mosquitto_strerror(rc));
  }
}

void http_processor(DataStructure *dstructure, std::string path, std::string payload) { dstructure->http_map[path] = payload; }

void get_instr(std::string op, std::string payload, OperationRegister *reg) {
  int ctrl;
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

  if (instr == "GET") {
    int detects = std::stoi(act);
    reg->input_data = strget(payload, detects);
  } else if (instr == "GET_FROM") {
    nlohmann::json json_obj;

    try {
      json_obj = nlohmann::json::parse(payload);
      if (!json_obj.is_object()) {
        // This shouldn't happen to be honest
        throw nlohmann::json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const nlohmann::json::parse_error &e) {
      spdlog::error("(De Ruyter) {}", e.what());
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
    } catch (const nlohmann::json::parse_error &e) {
      spdlog::error("(De Ruyter) {}", e.what());
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
      spdlog::error("(De Ruyter) {}", e.what());
      json_obj[act] = reg->input_data;
    }

    reg->convert = "";
    reg->output_data = json_obj.dump();
  } else if (instr == "CLCT_FROM") {
    nlohmann::json json_obj;

    try {
      json_obj = nlohmann::json::parse(payload);
      if (!json_obj.is_object()) {
        // This shouldn't happen to be honest
        throw nlohmann::json::parse_error::create(6767, 0, "Technically valid, but not an object dawg. Thus pizdec", nullptr);
      }
    } catch (const nlohmann::json::parse_error &e) {
      spdlog::error("(De Ruyter) {}", e.what());
    }

    std::string deref = json_obj[act];

    reg->output_data = deref;
  } else if (instr == "IF") {
    std::string compared_value = strget(act, 1);
    reg->logic_comparison = compare(act, std::stoi(reg->input_data), std::stoi(compared_value));
  } else if (instr == "CONV_TO") {
    reg->convert = act;
  } else if (instr == "THEN") {
    if (reg->logic_comparison)
      reg->output_data = act;
  } else if (instr == "OTHERWISE") {
    if (!reg->logic_comparison)
      reg->output_data = act;
  } else {
    spdlog::error("(Process Instr) Unrecognized instruction {}", instr);
  }
}

void comms_manager(DataStructure *dstructure, nlohmann::json *src, nlohmann::json *dst, std::string payload, std::string rule_file_location) {
  std::string src_conn = (*src)["device"]["connection"];
  std::string dest_conn = (*dst)["device"]["connection"];
  std::ifstream rule_file("./rules/" + rule_file_location);
  std::stringstream ss;

  std::string dst_name = (*dst)["name"];

  OperationRegister op_reg;

  std::string line;

  if (rule_file) {
    ss << rule_file.rdbuf();
    rule_file.close();

    while (std::getline(ss, line)) {
      get_instr(line, payload, &op_reg);
    }

  } else {
    spdlog::error("(De Ruyter) Rule file location not found? Perhaps a typo? Or maybe deliberate.");
  }

  std::string final_payload = !op_reg.output_data.empty() ? op_reg.output_data : payload;

  if (dest_conn == "wifi/http") {
    http_processor(dstructure, dst_name, final_payload);
  } else if (dest_conn == "wifi/mqtt") {
    mqtt_processor(dstructure, dst_name, final_payload);
  } else if (dest_conn == "ble") {
    ble_processor(dstructure, dst_name, final_payload);
  }
}

int de_ruyter(DataStructure *dstructure, std::string src, std::string payload) {
  if (!dstructure->connection_registers.count(src)) {
    spdlog::error("(DeRuyter) This source isn't listed: {}", src);
    return 1;
  }

  nlohmann::json connection = dstructure->connection_registers[src];

  std::string dst = connection["destination"];

  std::string rules = "";

  if (connection.count("rules") > 0) {
    rules = connection["rules"];
  }

  nlohmann::json test_src;
  nlohmann::json test_dst;

  dstructure->populate(&dstructure->instance_registers, src, &test_src, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  dstructure->populate(&dstructure->instance_registers, dst, &test_dst, std::make_pair("device", &dstructure->device_profiles), std::make_pair("format", &dstructure->format_profiles));

  comms_manager(dstructure, &test_src, &test_dst, payload, rules);

  return 0;
}
