#pragma once

#include "data_structure.hpp"
#include <iostream>

struct TaskData {
  std::string task_name;
  std::string source;
  int interval;
  int dummy_counter;
  std::thread *current_thread;
};

void generic_task_function(TaskData *td);
void create_task(DataStructure *ds, std::string task_name, std::string source, int interval, std::thread **ret_thr);
std::thread *create_task(DataStructure *ds, std::string task_name, std::string source, int interval);
void create_task_detached(DataStructure *ds, std::string task_name, std::string source, int interval);