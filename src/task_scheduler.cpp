#include "task_scheduler.hpp"
#include "data_route_handler.hpp"
#include <functional>
#include <iostream>
#include <thread>

void generic_task_function(DataStructure::TaskData *td [[maybe_unused]]) {
  //nlohmann::json interop_data;
  //bool success_create_register = TourDeScheduler::create_task_register(td->source, &interop_data);

////  spdlog::debug("Interop Data:\n{}", interop_data.dump(2));

  //while (!interop_data.empty() && td->active && success_create_register) {
  //  data_route_handler(td->ds, &interop_data, interop_data["rules"]);

  //  std::this_thread::sleep_for(std::chrono::milliseconds((int)interop_data["interval"]));
  //}

  //spdlog::info("Task {} is done", td->source);

  //if (td->ds != nullptr) {
  //  td->ds->active_registers.erase(td->source);
  //} else {
  //  spdlog::error("(Task) TaskData's DataStructure is undefined!");
  //}

  //delete td;
}

bool create_task_detached(DataStructure::Instance *ds [[maybe_unused]], std::string source [[maybe_unused]]) {
  // if (ds->active_registers.count(source)) {
  //   spdlog::error("Task with this name has already been created");
  //   return false;
  // }

  // DataStructure::TaskData *td = new DataStructure::TaskData();
  // td->source = source;
  // td->ds = ds;
  // td->active = true;

  // ds->active_registers[source] = td;

  // std::thread thr(generic_task_function, td);
  // thr.detach();

////   spdlog::info("Task {} is starting...", source);

  // return true;
  //
  return false;
}

