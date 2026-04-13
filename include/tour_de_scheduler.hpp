#pragma once

#include "data_structure.hpp"
#include "de_ruyter.hpp"
#include <functional>
#include <iostream>
#include <thread>

struct TaskData {
  std::string task_name;
  std::string source;
  int interval;
  int dummy_counter;
  std::thread *current_thread;
  DataStructure *ds;
};

void generic_task_function(TaskData *td);
bool create_task(DataStructure *ds, std::string task_name, std::string source, int interval, std::thread **ret_thr);
std::thread *create_task(DataStructure *ds, std::string task_name, std::string source, int interval);
bool create_task_detached(DataStructure *ds, std::string task_name, std::string source, int interval);