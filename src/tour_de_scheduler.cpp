#include "tour_de_scheduler.hpp"

void generic_task_function(TaskData *td) {
  int internal_counter = 0;

  nlohmann::json src, dst;

  td->ds->populate(&td->ds->instance_registers, td->source, &src, std::make_pair("device", &td->ds->device_profiles), std::make_pair("format", &td->ds->format_profiles));

  spdlog::debug("Source instance:\n{}", src.dump(1));

  while (internal_counter < td->dummy_counter) {
    spdlog::debug("Source: {}", td->source);

    // Lazy debil way
    if (td->ds->http_map.count(td->source)) {
      de_ruyter(td->ds, td->source, td->ds->http_map[td->source]);
    } else if (td->ds->mqtt_map.count(td->source)) {
      de_ruyter(td->ds, td->source, td->ds->mqtt_map[td->source]);
    } else if (td->ds->ble_map.count(td->source)) {
      de_ruyter(td->ds, td->source, td->ds->ble_map[td->source]);
    }

    internal_counter++;
    std::this_thread::sleep_for(std::chrono::milliseconds(td->interval));
  }

  if (td->ds != nullptr) {
    td->ds->active_registers.erase(td->task_name);
  } else {
    spdlog::error("(Task) TaskData's DataStructure is undefined!");
  }

  delete td;
}

bool create_task(DataStructure *ds, std::string task_name, std::string source, int interval, std::thread **ret_thr) {
  if (ds->active_registers.count(task_name)) {
    spdlog::error("Task with this name has already been created");
    return false;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 5;
  td->ds = ds;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  if (ret_thr != nullptr) {
    *ret_thr = thr;
  }

  return true;
}

std::thread *create_task(DataStructure *ds, std::string task_name, std::string source, int interval) {
  if (ds->active_registers.count(task_name)) {
    spdlog::error("Task with this name has already been created");
    return nullptr;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 5;
  td->ds = ds;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  return thr;
}

bool create_task_detached(DataStructure *ds, std::string task_name, std::string source, int interval) {
  if (ds->active_registers.count(task_name)) {
    spdlog::error("Task with this name has already been created");
    return false;
  }

  TaskData *td = new TaskData();
  td->task_name = task_name;
  td->source = source;
  td->interval = interval;
  td->dummy_counter = 5;
  td->ds = ds;

  std::thread *thr = new std::thread(generic_task_function, td);
  td->current_thread = thr;

  ds->active_registers[task_name] = thr;

  thr->detach();

  return true;
}