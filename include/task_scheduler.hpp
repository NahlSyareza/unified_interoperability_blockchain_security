#pragma once

#include "data_structure.hpp"

namespace TourDeScheduler {
void extract_config(std::string path, nlohmann::json *json_ptr);
bool create_task_register(std::string source, nlohmann::json *json_ptr);
}; // namespace TourDeScheduler

void generic_task_function(DataStructure::TaskData *td);
bool create_task_detached(DataStructure::Instance *ds, std::string source);
