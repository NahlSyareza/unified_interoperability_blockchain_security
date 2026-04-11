#include "service_scheduler.hpp"
#include <functional>
#include <iostream>
#include <thread>

void generic_task_function(TaskData *td) {
  int internal_counter = 0;

  while (internal_counter < td->dummy_counter) {
    spdlog::debug("Source: {}", td->source);

    internal_counter++;
    std::this_thread::sleep_for(std::chrono::milliseconds(td->interval));
  }

  delete td;
}

void create_task(DataStructure *ds, std::string task_name, std::string source, int interval, std::thread **ret_thr) {
  if (ds->active_registers.count(task_name) > 0) {
    spdlog::error("Task with this name has already been created");
    return;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 3;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  if (ret_thr != nullptr) {
    *ret_thr = thr;
  }
}

std::thread *create_task(DataStructure *ds, std::string task_name, std::string source, int interval) {
  if (ds->active_registers.count(task_name) > 0) {
    spdlog::error("Task with this name has already been created");
    return nullptr;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 3;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  return thr;
}

void create_task_detached(DataStructure *ds, std::string task_name, std::string source, int interval) {
  if (ds->active_registers.count(task_name) > 0) {
    spdlog::error("Task with this name has already been created");
    return;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 3;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  thr->detach();
}