cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_TYPE "Release")

project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(legendary_boss main.cpp src/ble_handler.cpp src/de_ruyter.cpp src/http_handler.cpp src/mqtt_handler.cpp src/spitarm.cpp src/tour_de_scheduler.cpp src/data_structure.cpp src/rf24_handler.cpp src/uart_handler.cpp src/i2c_handler.cpp src/gpio_handler.cpp)

# RF24
find_path(RF24_INCLUDE RF24.h PATH_SUFFIXES RF24)
find_library(RF24_LIB NAMES rf24)
target_include_directories(legendary_boss PRIVATE ${RF24_INCLUDE})
target_link_libraries(legendary_boss PRIVATE ${RF24_LIB})

# Threads or Pthreads I think
find_package(Threads REQUIRED)
target_link_libraries(legendary_boss PRIVATE Threads::Threads)

# Mosquitto
find_package(PkgConfig REQUIRED)
pkg_check_modules(mosquitto REQUIRED libmosquitto)
target_link_libraries(legendary_boss PRIVATE ${mosquitto_LIBRARIES})
# target_link_directories(legendary_boss PRIVATE ${mosquitto_LIBRARY_DIRS}) 
target_include_directories(legendary_boss PRIVATE ${mosquitto_INCLUDE_DIRS})

# GPIODCXX
# find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIODCXX REQUIRED IMPORTED_TARGET libgpiodcxx>=2.0)
target_link_libraries(legendary_boss PRIVATE PkgConfig::GPIODCXX)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(legendary_boss PRIVATE OpenSSL::SSL)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE spdlog::spdlog)

# SimpleBluez and SimpleDBus
find_package(simplebluez CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE simplebluez::simplebluez)

add_compile_definitions(FMT_HEADER_ONLY)

# cpp-httplib
# find_package(PkgConfig REQUIRED)
pkg_check_modules(HTTPLIB REQUIRED IMPORTED_TARGET cpp-httplib)
target_link_libraries(legendary_boss PRIVATE PkgConfig::HTTPLIB)

# json
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(legendary_boss PRIVATE nlohmann_json::nlohmann_json)

# Includes all the local header files
target_include_directories(legendary_boss PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# if(NOT WIN32)
target_link_options(legendary_boss PRIVATE "-pthread")
# endif()

# Extreme mode
target_compile_options(legendary_boss PRIVATE -Wall -Wextra -Werror)
