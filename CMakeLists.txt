cmake_minimum_required(VERSION 3.15)
project(legendary_boss)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(mosquitto REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(legendary_boss main.cpp)

target_include_directories(legendary_boss PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(legendary_boss PRIVATE
    mosquitto::mosquitto
    spdlog::spdlog
    fmt::fmt
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

target_link_options(legendary_boss PRIVATE "-pthread")