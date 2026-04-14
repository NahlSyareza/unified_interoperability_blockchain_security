#include "tour_de_scheduler.hpp"

void generic_task_function(DataStructure::TaskData *td) {
  nlohmann::json interop_data;
  TourDeScheduler::create_task_register(td->source, &interop_data);

  // spdlog::debug("Interop Data:\n{}", interop_data.dump(2));

  while (!interop_data.empty() && td->active) {
    de_ruyter(td->ds, &interop_data, interop_data["rules"]);

    std::this_thread::sleep_for(std::chrono::milliseconds((int)interop_data["interval"]));
  }

  spdlog::info("Task {} is done", td->task_name);

  if (td->ds != nullptr) {
    td->ds->active_registers.erase(td->task_name);
  } else {
    spdlog::error("(Task) TaskData's DataStructure is undefined!");
  }

  delete td;
}

bool create_task_detached(DataStructure::Instance *ds, std::string task_name, std::string source) {
  if (ds->active_registers.count(task_name)) {
    spdlog::error("Task with this name has already been created");
    return false;
  }

  DataStructure::TaskData *td = new DataStructure::TaskData();
  td->task_name = task_name;
  td->source = source;
  td->ds = ds;
  td->active = true;

  ds->active_registers[task_name] = td;

  std::thread thr(generic_task_function, td);
  thr.detach();

  spdlog::info("Task {} is starting...", task_name);

  return true;
}